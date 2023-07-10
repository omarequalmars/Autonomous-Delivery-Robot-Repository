import qrcode

# Text to be encoded in the QR code
text = "not valid"

# Create a QR code instance
qr = qrcode.QRCode(version=7, box_size=10, border=4)

# Add the text to the QR code
qr.add_data(text)

# Generate the QR code
qr.make(fit=True)

# Create an image from the QR code
img = qr.make_image(fill_color="black", back_color="white")

# Save the image to a file
img_path = r'C:\Users\E\Desktop\Encoder\QR\qr_code7.png'
img.save(img_path)