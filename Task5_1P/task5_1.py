from PyQt5 import QtWidgets
from PyQt5.QtWidgets import QApplication, QMainWindow, QRadioButton, QPushButton
import sys
import RPi.GPIO as GPIO

led_hallway = 19
led_bathroom = 21
led_closet = 23

GPIO.setmode(GPIO.BOARD)
GPIO.setup(led_hallway, GPIO.OUT)
GPIO.setup(led_bathroom, GPIO.OUT)
GPIO.setup(led_closet, GPIO.OUT)

class MyWindow(QMainWindow):
    def __init__(self):
        super(MyWindow, self).__init__()
        self.setGeometry(300, 300, 600, 600)
        self.setWindowTitle("Task5.1P")
        self.initUI()

    def initUI(self):
        self.rb1 = QRadioButton("Hallway", self)
        self.rb1.move(50,50)
        self.rb1.toggled.connect(self.radio_selected)

        self.rb2 = QRadioButton("Bathroom", self)
        self.rb2.move(50,100)
        self.rb2.toggled.connect(self.radio_selected)
        
        self.rb3 = QRadioButton("Closet", self)
        self.rb3.move(50,150)
        self.rb3.toggled.connect(self.radio_selected)
   
    def radio_selected(self):
        selected = self.sender()
        
        if selected.isChecked():
            room = selected.text()
            GPIO.output(led_hallway, GPIO.LOW)
            GPIO.output(led_bathroom, GPIO.LOW)
            GPIO.output(led_closet, GPIO.LOW)
            
            if room == "Hallway":
                GPIO.output(led_hallway, GPIO.HIGH)
            elif room == "Bathroom":
                GPIO.output(led_bathroom, GPIO.HIGH)
            else:
                GPIO.output(led_closet, GPIO.HIGH)

    def closeEvent(self, event):
        GPIO.cleanup()
        event.accept()

def window():
    app = QApplication(sys.argv)
    win = MyWindow()
    win.show()
    sys.exit(app.exec_())

window()

