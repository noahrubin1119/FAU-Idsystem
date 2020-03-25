from urllib.request import urlopen
from imutils.video import VideoStream
from pyzbar import pyzbar
import datetime
import imutils
import time
import cv2


print("starting video")
print("Register New Student/s")
print("Complete Form: ")

vs = VideoStream().start()
time.sleep(2.0)


while True:
	frame = vs.read()
	frame = imutils.resize(frame, width=400)

	barcodes = pyzbar.decode(frame)
	# loop over the detected barcodes
	for barcode in barcodes:
		# extract the bounding box location of the barcode and draw
		# the bounding box surrounding the barcode on the image
		(x, y, w, h) = barcode.rect
		cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 0, 255), 2)

		# the barcode data is a bytes object so if we want to draw it
		# on our output image we need to convert it to a string first
		barcodeData = barcode.data.decode("utf-8")
		barcodeType = barcode.type

		# draw the barcode data and barcode type on the image
		text = "{} ({})".format(barcodeData, barcodeType)
		cv2.putText(frame, text, (x, y - 10),
		cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2)
		questionMark = len(barcodeData)

		barcodeData = barcodeData[0:questionMark]

		print("Swipe your card")

		magstrip = input("Swipe")
		questionMark = len(magstrip) - 1

		magstrip = magstrip[1:questionMark]


		url = "https://script.google.com/macros/s/AKfycbxFjSuWk3HR-m_b2GOBr7o5M0UlgzJGnUwOWrhDHkGYD6Wr0T-k/exec?id=Att&item1" + magstrip + "&item2=" + barcodeData;

		print(url) 
		urlopen(url)
		
	# show the output frame
	cv2.imshow("Barcode Scanner", frame)
	key = cv2.waitKey(1) & 0xFF
 
	# if the `q` key was pressed, break from the loop
	if key == ord("q"):
		break
 

cv2.destroyAllWindows()
vs.stop()
