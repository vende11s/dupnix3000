# dupnix3000
## Description
* This trojan gives you full control of the victim pc
* I recommend you make your pc victim to test it
* This software probably has some bugs, please report them at the issues

## Using
* [How does it works](#how-does-it-works)
* [Create and set a telegram bot](#Create-and-set-a-telegram-bot)
* [Set const variables](#Set-const-variables)
* [Compile](#Compile)
* [Install on victim pc](#install-on-victim-pc)
* [Autostart](#Autostart)
* [commands](#commands)
* [About project](#About-Project)
* [Disclaimer](#Disclaimer)
### How does it works
* You send [commands](#commands) with an ID on telegram chat with bot and then virus does stuff
* Every infected pc has its own ID, default it's `X`, you can set it by X SetID <ID>
* example usage: `420 SetVolume 69`, `420` - ID, `SetVolume` - command, `69` - value for command
####  How does it ✨actually✨ work
*  When you send a message on a telegram channel with the bot, the message is avaible at `https://api.telegram.org/bot<botToken>/getUpdates`
*  The trojan downloads it using [cpr](https://github.com/whoshuu/cpr) then parsing it and if ID from message match with local ID, it performs a [command](#commands)
*  As you can see it's very simple  

### Create and set a telegram bot
#### Create your bot
* First in Telegram's search bar write `@BotFather`
* In chat with `BotFather` text him `/newbot`
* Name your bot
* `BotFather` will send you your api token, save it somewhere, you'll need it later
* Text to BotFather `/setprivacy`
* Choose your bot
* Text him `Disable`
#### Create a channel
* Create a new channel
* Add to this channel your bot as admin

### Set const variables
* First of all you should send any message on your telegram channel
* Then go to `https://api.telegram.org/bot<api Token>/getUpdates`  Your token is [there](#Create-and-set-a-telegram-bot) 
* Find the `"id":`numbers after that is your `troj.hpp -> chat_id`
* put your api token to `troj.hpp -> botApi`
* go to main.cpp and set if it should use [autostart](#autostart) and if it should hide the command line window

### Compile
#### MSVC
* Download [cpr](https://github.com/whoshuu/cpr), [json](https://github.com/nlohmann/json), [opencv](https://github.com/opencv/opencv) using vcpkg, (download it stacically too, if you want)
* Then Compile, job done, it's time for CounterStrike
* If you have problem with compiling or anything else DM me on Discord (my tag is in github profile description)

### Install on victim pc
* Put it anywhere you want (If you have turned on [autostart](#autostart) it will copy itself to %temp% else you have to hide it manually and add it to autostart)
* Run it
* Set ID in telegram channel
* enjoy causing chaos, use your [commands](#commands) on victim pc

### Autostart
* if you have turned it on you can put your executable whenever you want, it will automaticly copy it self to %temp% and add to autostart
* after that it will send info about new user, username and public ip

### Commands
* Before all commands should be an ID, default ID is `X` but use it only for `SetID`
* `<ID> <command> <parameters>`, you can use && when you want 2 commands in one message, e.g 420 SetCursor 1920,0 && Hotkey lmouse
* You can also use: `$UPTIME$`,`$EXE_NAME$`,`$HOST_NAME$`,`$PUBLIC_IP$`,`$ADIMN_RIGHTS$`,`$EXE_PATH$`,`$CURSOR_POSITION$`
### All available commands:
#### Status
* Shows a lot of useful info
#### ![image](https://user-images.githubusercontent.com/74505994/146406578-7aa5e4db-b212-4538-9222-940f0f7e20bf.png)
#### cmd <command>
 * Performs a command, e.g `cmd shutdown /h`
#### SetCursor <X,Y>
 * Sets Cursor Position
#### BlockCursor <seconds>
 * Blocks Cursor at 0,0 for <seconds>
#### Volume <0-100>, +<0-100>, -<0-100>, empty
 * You can set volume `Volume 69`
 * Or Increase/Decrease Volume, `Volume +69`, `Volume -69`
 * Or just see what's actual volume, `Volume`
#### BlockClipboard <true/fase>
 * Blocks using clipboard
#### SetID <ID>
 * Sets ID 
#### Press <text>
 * You can write sth using Press, `Press fuckdis`
#### SendClipboard
 * Shows what is in victim's clipboard
#### Screenshot
 * Shows you what victim see on screen
#### msgbox <\"Text\",number,\"Title\">
 * Makes a msgbox
#### Hotkey <hotkey>
 * you can perform some hotkey, e.g `ctrl+z`
 * List of all special keys: 
 ```esc
 tab
 capslock
 shift
 ctrl
 win
 alt
 ralt
 space
 enter
 backspace
 del
 f1
 f..12
 ins
 home
 end
 pgdn
 pgup
 uparrow
 downarrow
 leftarrow
 rightarrow
 lmouse 
 ```
#### Taskkill <proccess name+.exe>
 * kills a process, e.g Taskkill notepad.exe 
#### ProcessList
 * List of all processes
#### IsFileExits <path to file>
 * Checks if file exists
#### WebcamView
 * Shows view from webcam
#### Sleep <seconds>
 * Make dupnix sleeping for some time
 * It may be useful to plan sth, e.g `Sleep 3600 && shutdown /h`
#### ErrorSound
 * Just makes an ErrorSound
#### RunningApps
 * List of running first-plan apps
#### ![image](https://user-images.githubusercontent.com/74505994/146406835-d8de34ef-eef3-4928-a93c-2bc80eeca871.png)
#### ALL_ID (without <id>)
 * every victim sends its id, your not using <id>, just `ALL_ID`
#### ListOfFiles <path>
 * Sends you every file in given directory
#### ![image](https://user-images.githubusercontent.com/74505994/146407474-0aca95b0-220c-4f01-885f-b1e0c51a6792.png)
#### WifiList
 * Gives you a list of Wifi names and passwords for them
#### CloseForeground <true/false>
 * Pretty cool troll which closing every window you click at
#### WriteToClipboard
 * You can put sth inside the clipboard
 
#### Delitself
 * Remote deleting dupnix from victim's pc
#### Update <link to new version>
 * Remote Update
#### help
 * List of all commands

### About Project
 Actually I'm developing it solo but if you want to help me with this half shitty and half stackoverflow code text me on discord (my tag in github profile description), if you have any problems with using this soft text me too
### Disclaimer
Whole this project is only for eductional purposes, I am not responsible for anything you did or will do with this software
