import cv2
import numpy as np
import os

# --- Configuration ---
VIDEO_FILE = 'bad_apple.mp4'
OUTPUT_FILE = 'badapple.bin'
WIDTH = 128
HEIGHT = 32
TARGET_FPS = 10  # Updated to 10 FPS

def convert():
    if not os.path.exists(VIDEO_FILE):
        print(f"Error: {VIDEO_FILE} not found!")
        return

    cap = cv2.VideoCapture(VIDEO_FILE)
    source_fps = cap.get(cv2.CAP_PROP_FPS)
    total_source_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
    
    # Calculate how many frames to skip to hit 10 FPS
    # e.g., if source is 30fps, hop = 3 (take every 3rd frame)
    hop = max(1, source_fps / TARGET_FPS)
    
    print(f"Source: {source_fps:.2f} FPS | Target: {TARGET_FPS} FPS")
    print(f"Processing entire video (approx {total_source_frames / source_fps:.1f} seconds)")

    processed_count = 0
    current_frame_idx = 0.0  # Use float for more accurate timing

    with open(OUTPUT_FILE, 'wb') as f:
        while True:
            # Set the position to the next frame we want based on TARGET_FPS
            cap.set(cv2.CAP_PROP_POS_FRAMES, int(current_frame_idx))
            ret, frame = cap.read()
            
            if not ret:
                break
            
            # 1. Grayscale and Resize
            gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            resized = cv2.resize(gray, (WIDTH, HEIGHT), interpolation=cv2.INTER_AREA)
            
            # 2. Binary Threshold
            _, bw = cv2.threshold(resized, 127, 255, cv2.THRESH_BINARY)

            # 3. SSD1306 Page Packing (Vertical bytes)
            # We iterate through 'pages' (8-pixel high strips)
            for page in range(HEIGHT // 8): 
                for x in range(WIDTH):      
                    byte = 0
                    for bit in range(8):
                        if bw[page * 8 + bit, x] > 0:
                            byte |= (1 << bit)
                    f.write(bytes([byte]))
            
            processed_count += 1
            current_frame_idx += hop
            
            if processed_count % 100 == 0:
                print(f"Progress: {processed_count} frames converted...", end='\r')

    cap.release()
    
    final_size_kb = os.path.getsize(OUTPUT_FILE) / 1024
    print(f"\n\n--- DONE ---")
    print(f"Frames Created: {processed_count}")
    print(f"Final File Size: {final_size_kb:.2f} KB")
    # QMK/ProMicro limit check (usually ~128KB total flash, but depends on your MCU)
    print(f"Status: {'READY' if final_size_kb < 1024 else 'LARGE FILE - Check MCU Capacity'}")

if __name__ == "__main__":
    convert()