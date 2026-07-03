Imitator game

How to use:
When you power the arduino, the LCD screen will show you have not recorded anything yet. To record something, press the latch switch. The LCD screen will show a countdown
starting from 3. Once you do that, you can press the buzzer and green led buttons as much as you like. The maximum recording time is 10s, at which you will be asked to
press the latch switch to stop the program. To end it before the 10s mark, just press the latch switch. Afterwards press the buzzer and led buttons together to replay
your actions exactly. You can re-record and overide your previous one if you press the latch switch again.

How it works:
When you start recording, the arduino will store in a list every millisecond whether something is being pressed or not. In another list, at the exact same time, it is storing 
the exact millisecond. Since the two lists have items added at the same time, you can figure out when something is pressed in the time list. This is part of the reason why I 
can only make it last 10s, the arduino's memory gets pushed to the max at 12s and crashes afterwards.

There is a way to fix it, it involves getting rid of the time list completely and just going with the button pressed list. You have to assume that each while loop will run the
exact same time. But obviously it doesn't make it exact so i opted quality over quantity.

When you replay it, it just looks at every corresponding item in both lists and replays them. The function has a timer already running when it starts, and it makes sure it
only plays the buzzer or led when the timer matches the timing in the list, making it perfect every time.
