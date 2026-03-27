from PIL import Image
import serial


print("Hello!")
image = Image.open('Smith_Matrix.jpg')
width, height = image.size
width_disp, height_disp=320, 240
ser=serial.Serial(port="Com4", baudrate=115200, timeout=0.0)
if ser.is_open:
    print(f"Port {ser.name} open")
else:
    print(f"Port {ser.name} doesn't open")

try:
    x1=width/320
    y1=height/240
    for i in range(width_disp):
        for j in range(height_disp):
            rgb_im = image.convert('RGB')
            r, g, b = rgb_im.getpixel((i, j))
            #pixel_color = image.getpixel((i, j))
            #hex_color = '#{:02X}{:02X}{:02X}'.format(pixel_color)
            print(r, g, b);

finally:
    ser.close();
    print("Port closed")
