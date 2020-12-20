import socket
import sys
import json
import smtplib

#Change this address to the IP address of the computer that is running the server

RPIADDR= '10.0.0.248'
LOCAL = '67.185.221.70'
#creates a TCP socket, we might end up using TCP, but we will see about the future
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#Using the socket object we created, connect to the server IP at the given port number
s.connect((RPIADDR,9999))

count = 0
xOriginal = ""

email_user = 'enter-email'
email_send = 'enter-email'

server = smtplib.SMTP('smtp.gmail.com',587)
server.starttls()
server.login(email_user, 'enter-password')

message = 'Someone is in your room!!'


while True:
    recMessage = s.recv(256).decode("utf-8")
    count += 1
    #if we actually received something from the server, print it out and continue
    if(len(recMessage) > 0):
        recMessage = recMessage.split()
        x = recMessage[0]
        y = recMessage[1]
        z = recMessage[2]
        xString = "" + x
        xString = int(xString)
        print(f"x: {x}\ty: {y}\tz: {z}")
        
        if(count == 5):
            xOriginal = "" + x
            xOriginal = int(xOriginal)
    #Else, something went wrong
    #TCP creates a 'tunnell' so if you don't get anything back that means your connection failed
    #if it does fail, you have to close everything and restart
    else:
        print(f"Error receiving, closing")
        sys.exit()

    print(f"count: {count}  currentX: {xString}  xOriginal: {xOriginal}")
    
    if(count > 5 and ((xOriginal == xString) or (xString <= xOriginal + 100))):
        print("No Change")
    elif(count > 5 and ((xOriginal != xString) or (xString > xOriginal + 100) or (xString < xOriginal - 100))):
        print("Movement")
        server.sendmail(email_user, email_send, message)
        server.quit()
        sys.exit()



