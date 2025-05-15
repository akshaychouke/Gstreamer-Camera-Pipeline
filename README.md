# 🎥 Real-time Camera Pipeline using GStreamer in C

This project is a real-time video processing application built with **GStreamer** in **C**. It captures video from a webcam and allows users to apply dynamic transformations like **cropping** and **flipping** during runtime using simple terminal commands.

## 📌 Features

- 📸 Capture live video from a V4L2-compatible camera (`/dev/video0`)
- 🧩 Decode MJPEG streams using `jpegdec`
- ✂️ Dynamically crop video using `videocrop`
- 🔁 Flip video in real-time using `videoflip` (horizontal, vertical, 180° rotation)
- 🧼 Video format conversion using `videoconvert`
- 📺 Display video using `autovideosink`
- 🧑‍💻 Real-time terminal-based input to control transformations

## 🛠️ Tech Stack

- **Language:** C
- **Multimedia Framework:** [GStreamer](https://gstreamer.freedesktop.org/)
- **Linux Interface:** [Video4Linux2 (v4l2)](https://www.kernel.org/doc/html/latest/userspace-api/media/v4l/v4l2.html)

## 📦 Dependencies

Ensure you have the Gstreamer installed.

## 🧪 Build Instructions
```bash
git clone https://github.com/yourusername/gstreamer-realtime-camera-pipeline.git
cd gstreamer-realtime-camera-pipeline
gcc main.c -o camera-pipeline `pkg-config --cflags --libs gstreamer-1.0`
```

## 🚀 Run the Application
```bash
./camera-pipeline
```
Make sure your webcam is accessible via /dev/video0.

## 🖥️ Controls (Runtime Terminal Commands)

f → Flip the video (cycles through flip modes)

t → Set top crop

b → Set bottom crop

l → Set left crop

r → Set right crop

q → Quit the application