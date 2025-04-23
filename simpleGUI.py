import serial
import time  
import socket
import tkinter as tk 
import threading
import os 
def main():

        global window  
        window = tk.Tk() 

        
        global Last_command_Label  
        Last_command_Label = tk.Label(text="Last Command Sent: ") 
        Last_command_Label.pack() 

       
        quit_command_Button = tk.Button(text ="Press to Quit", command = send_quit)
        quit_command_Button.pack()  

        
        scan_command_Button = tk.Button(text ="Press to Autoscan", command = send_autoscan)
        scan_command_Button.pack() 

        
        scan_command_Button = tk.Button(text ="Press to Toggle", command = send_toggle)
        scan_command_Button.pack() 

       
        scan_command_Button = tk.Button(text ="Press to Move Forward", command = send_moveforward)
        scan_command_Button.pack() 

       
        scan_command_Button = tk.Button(text ="Press to Rotate Left", command = send_rotateleft)
        scan_command_Button.pack() 

        
        scan_command_Button = tk.Button(text ="Press to Move Backward", command = send_moveback)
        scan_command_Button.pack() 

        
        scan_command_Button = tk.Button(text ="Press to Rotate Right", command = send_rotateright)
        scan_command_Button.pack() 

       
        scan_command_Button = tk.Button(text ="Press to Scan", command = send_scan)
        scan_command_Button.pack() 
        my_thread = threading.Thread(target=socket_thread) 
        my_thread.start() 

        
        window.mainloop()



def send_quit():
        global gui_send_message
        global window  
        
        gui_send_message = "quit\n"   
        time.sleep(1)
        window.destroy() 



def send_autoscan():
        global gui_send_message 
        
        gui_send_message = "h\n"   

def send_toggle():
        global gui_send_message 
        
        gui_send_message = "t\n"   

def send_moveforward():
        global gui_send_message 
        
        gui_send_message = "w\n"   

def send_rotateleft():
        global gui_send_message 
        
        gui_send_message = "a\n"   

def send_moveback():
        global gui_send_message 
        
        gui_send_message = "s\n"  

def send_rotateright():
        global gui_send_message 
        
        gui_send_message = "d\n"  

def send_scan():
        global gui_send_message
        
        gui_send_message = "m\n" 


def socket_thread():
        
        global Last_command_Label 
        global gui_send_message   

 
        absolute_path = os.path.dirname(__file__) 
        relative_path = "./"   
        full_path = os.path.join(absolute_path, relative_path) 
        filename = 'sensor-scan.txt' 

        HOST = "192.168.1.1"  
        PORT = 288       
        cybot_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  
        cybot_socket.connect((HOST, PORT))  
                      
        cybot = cybot_socket.makefile("rbw", buffering=0)  
        send_message = "h\n"                            
        gui_send_message = "wait\n"                                

        cybot.write(send_message.encode()) 

        print("Sent to server: " + send_message) 

        
        while send_message != 'quit\n':
                
                
                command_display = "Last Command Sent:\t" + send_message
                Last_command_Label.config(text = command_display)  
        
             
                if (send_message == "h\n") or (send_message == "m\n"):

                        print("Requested Sensor scan from Cybot:\n")
                        rx_message = bytearray(1) 

                   
                        file_object = open(full_path + filename,'w')
                        i = 0
                        while (i < 95): 
                                rx_message = cybot.readline()   
                                file_object.write(rx_message.decode())  
                                print(rx_message.decode()) 
                                i += 1

                        file_object.close()              

                else:                
                        print("Waiting for server reply\n")
                        rx_message = cybot.readline()      
                        print("Got a message from server: " + rx_message.decode() + "\n") 


                
                while gui_send_message == "wait\n": 
                        time.sleep(.1)  
                        send_message = gui_send_message

               
                gui_send_message = "wait\n"                         

                cybot.write(send_message.encode()) 
                
        print("Client exiting, and closing file descriptor, and/or network socket\n")
        time.sleep(2) 
        cybot.close() 
        cybot_socket.close()  


main()