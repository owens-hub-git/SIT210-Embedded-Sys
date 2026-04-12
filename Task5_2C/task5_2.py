from PyQt5 import QtWidgets
from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import QApplication, QMainWindow, QRadioButton, QPushButton
import sys
import RPi.GPIO as GPIO
##
led_hallway = 19
led_bathroom = 21
led_closet = 23
led_livingroom = 32

GPIO.setmode(GPIO.BOARD)
GPIO.setup(led_hallway, GPIO.OUT)
GPIO.setup(led_bathroom, GPIO.OUT)
GPIO.setup(led_closet, GPIO.OUT)
GPIO.setup(led_livingroom, GPIO.OUT)

pwm_led = GPIO.PWM(led_livingroom, 1000)
pwm_led.start(50)

class MyWindow(QMainWindow):
    def __init__(self):
        super(MyWindow, self).__init__()
        self.setGeometry(300, 300, 800, 400)
        self.setWindowTitle("Task5.1P")
        self.initUI()

    def initUI(self):
        self.slider = QtWidgets.QSlider(Qt.Horizontal, self)
        self.slider.resize(500, 50)
        self.slider.move(50,50)
        self.slider.setMinimum(0)
        self.slider.setMaximum(100)
        self.slider.setValue(50)
        self.slider.valueChanged.connect(self.slider_change)
        
        self.rb1 = QtWidgets.QRadioButton(self)
        self.rb1.setText("Hallway")
        self.rb1.move(50,100)
        self.rb1.toggled.connect(self.radio_selected)

        self.rb2 = QtWidgets.QRadioButton(self)
        self.rb2.setText("Bathroom")
        self.rb2.move(50,150)
        self.rb2.toggled.connect(self.radio_selected)

        self.rb3 = QtWidgets.QRadioButton(self)
        self.rb3.setText("Closet")
        self.rb3.move(50,200)
        self.rb3.toggled.connect(self.radio_selected)

        self.pb1 = QtWidgets.QPushButton(self)
        self.pb1.setText("Exit")
        self.pb1.move(50,250)
        self.pb1.clicked.connect(self.exit_button_click)
    
    def slider_change(self):
        value = self.slider.value()
        #print(value)
        pwm_led.ChangeDutyCycle(value)
   
    def exit_button_click(self):
        self.close()

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
        GPIO.output(led_hallway, GPIO.LOW)
        GPIO.output(led_bathroom, GPIO.LOW)
        GPIO.output(led_closet, GPIO.LOW)
        pwm_led.stop()
        GPIO.output(led_livingroom, GPIO.LOW)
        GPIO.cleanup()
        event.accept()

def window():
    app = QApplication(sys.argv)
    win = MyWindow()
    win.show()
    sys.exit(app.exec_())

window()

