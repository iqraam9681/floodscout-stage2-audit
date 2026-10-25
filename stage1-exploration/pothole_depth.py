"""
Pothole Water-Depth Estimation Pipeline
Platform: NVIDIA Jetson Nano + Intel RealSense D455f
"""
import numpy as np
import cv2
import pyrealsense2 as rs
from collections import deque

GREEN_MAX = 3.0    # cm (Safe threshold)
AMBER_MAX = 8.0    # cm (Caution threshold)
REFRACT = 1.33     # Water refraction index correction
PAD = 60           # Margin for road plane reference ring

W, H = 640, 480

pipe = rs.pipeline()
cfg = rs.config()
cfg.enable_stream(rs.stream.depth, W, H, rs.format.z16, 30)
cfg.enable_stream(rs.stream.color, W, H, rs.format.bgr8, 30)
prof = pipe.start(cfg)

sensor = prof.get_device().first_depth_sensor()
if sensor.supports(rs.option.emitter_enabled):
    sensor.set_option(rs.option.emitter_enabled, 1)
scale = sensor.get_depth_scale()
intr = prof.get_stream(rs.stream.depth).as_video_stream_profile().get_intrinsics()


def get_points(depth, x0, y0, x1, y1, step=3):
    x0, y0 = max(x0, 0), max(y0, 0)
    x1, y1 = min(x1, W), min(y1, H)
    if x1 <= x0 or y1 <= y0:
        return np.zeros((0, 3), np.float32)
    d = depth[y0:y1:step, x0:x1:step].astype(np.float32) * scale
    v, u = np.mgrid[y0:y1:step, x0:x1:step]
    m = d > 0.1
    z = d[m]
    x = (u[m] - intr.ppx) / intr.fx * z
    y = (v[m] - intr.ppy) / intr.fy * z
    return np.stack([x, y, z], axis=1)


def fit_plane(P):
    c = P.mean(axis=0)
    n = np.linalg.svd(P - c)[2][-1]
    for _ in range(3):
        if n[2] > 0:
            n = -n
        r = np.abs((P - c) @ n)
        keep = r < max(np.percentile(r, 80), 0.005)
        if keep.sum() < 30:
            break
        c = P[keep].mean(axis=0)
        n = np.linalg.svd(P[keep] - c)[2][-1]
    if n[2] > 0:
        n = -n
    return c, n


def pick_roi():
    for _ in range(10):
        f = pipe.wait_for_frames()
    img = np.asanyarray(f.get_color_frame().get_data())
    x, y, w, h = cv2.selectROI("Select pothole, press ENTER", img)
    cv2.destroyWindow("Select pothole, press ENTER")
    return x, y, x + w, y + h


x0, y0, x1, y1 = pick_roi()
hist = deque(maxlen=7)

try:
    while True:
        frames = pipe.wait_for_frames()
        df = frames.get_depth_frame()
        cf = frames.get_color_frame()
        if not df or not cf:
            continue
        depth = np.asanyarray(df.get_data())
        img = np.asanyarray(cf.get_data()).copy()

        ox0, oy0 = x0 - PAD, y0 - PAD
        ox1, oy1 = x1 + PAD, y1 + PAD

        ring = np.vstack([
            get_points(depth, ox0, oy0, ox1, y0),
            get_points(depth, ox0, y1, ox1, oy1),
            get_points(depth, ox0, y0, x0, y1),
            get_points(depth, x1, y0, ox1, y1),
        ])
        roi = get_points(depth, x0, y0, x1, y1)

        if len(ring) < 200 or len(roi) < 50:
            status, color, ansi = "NO DEPTH DATA - TREAT AS RED", (0, 0, 255), "\033[91m"
            d_cm = float("nan")
        else:
            c, n = fit_plane(ring)
            drop = -((roi - c) @ n)
            d = max(np.percentile(drop, 95) * 100 * REFRACT, 0)
            hist.append(d)
            d_cm = float(np.median(hist))
            if d_cm < GREEN_MAX:
                status, color, ansi = "GREEN - SAFE", (0, 200, 0), "\033[92m"
            elif d_cm < AMBER_MAX:
                status, color, ansi = "AMBER - BE CAREFUL", (0, 191, 255), "\033[93m"
            else:
                status, color, ansi = "RED - DO NOT CROSS", (0, 0, 255), "\033[91m"

        print("{}{}  depth~{:.1f} cm\033[0m        ".format(ansi, status, d_cm), end="\r")

        cv2.rectangle(img, (x0, y0), (x1, y1), color, 2)
        cv2.rectangle(img, (0, 0), (W, 50), color, -1)
        cv2.putText(img, "{}  {:.1f}cm".format(status, d_cm), (10, 35),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.9, (255, 255, 255), 2)
        cv2.imshow("pothole", img)

        k = cv2.waitKey(1) & 0xFF
        if k == 27:
            break
        if k in (ord("r"), ord("R")):
            x0, y0, x1, y1 = pick_roi()
            hist.clear()
finally:
    pipe.stop()
    cv2.destroyAllWindows()
    print()
