Video Streaming Server Software for use with LM3S6965 Ethernet Evaluation Board.
================================================================================

This application will first convert normal video file to 128x96x4 bitmap data, 
then it will stream stream the converted data to the LM3S6965 board. 

Control Protocol Description
----------------------------

All commands will answer OK if successful or NOK if not successful. Unless 
otherwise specified. 
All commands should be ended by a single "\n".

* toggle: Will play or pause video depending on the current state.
* stop: Stop playing video. Current video cannot be resumed. 
* play <name>: Will play a file with the specified name.
* getplaylist: Wilt answer with a complete list of items currently in the 
   playlist.
* gettotal [<name>]: Returns the total time of <name>. Returns total time of
   current video if no name is given.
* getcurrent: Return the current position in the video.
* next: Plays the next item in the playlist.
* prev: Plays the previous item in the playlist.
* remove <name>: Will remove item <name> from the playlist.
  

