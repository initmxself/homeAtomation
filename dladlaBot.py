from IGNORE import dladla_secrets 
from UTILS import hcommands
import telepot
import serial
import time

cmds = hcommands

#open serial port at a baud rate of 9600 and set timeout to 1s
serArdu = serial.Serial('/dev/ttyACM0',9600, timeout=1) 
serArdu.flush() # Buffering, required to get the data out *now*

def handle(msg):
    """
    Handler function for incoming messages
    """

    # Get sender details
    user_name = msg["from"]["first_name"]

    # Get message details
    content_type, chat_type, chat_id = telepot.glance(msg)

    # Proceed if message type is text
    if content_type == "text":
        command = msg["text"].lower()

        """ 
	Check if the command is predefined and fetch response. Replace
	the chat_id with that of your group. I hardcoded so that the bot
	receives commands only and only from the specified group.
	"""
        if (command in cmds and (chat_id==-1234567890123):
            to_send = cmds[command] + '\n'
            serArdu.write(to_send.encode('utf-8'))
            print(" Command to send: " + to_send)
            time.sleep(5)
            response = serArdu.read(120).decode('utf-8')
        else:
            response = "Sorry {}. Command not found 😔".format(
                user_name)
        print(bot.sendMessage(chat_id, response))

if __name__ == "__main__":
    """ Starting block """

    # Read the API token from untracked file
    token = dladla_secrets

    # Instantiate the telegram bot
    bot = telepot.Bot(token)

    # Add handler to handle incoming messages
    bot.message_loop(handle)
    
    while 1:
        time.sleep(50)
