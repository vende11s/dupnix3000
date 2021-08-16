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
* To directory with `troj.hpp` and `main.cpp` put [cpr](https://github.com/whoshuu/cpr) lib and [json](https://github.com/nlohmann/json) lib
* Compile in `C++17`
* For G++, use `g++ trojan.cpp -march=native -O2 -static`
* If you have problem with compiling or anything else DM me on Discord (my tag is in github profile description)

### Install on victim pc
* Put it anywhere you want (If you have turned on [autostart](#autostart) it will copy itself to %temp% else you have to hide it manually and add it to autostart)
* Run it
* Set ID in telegram channel
* enjoy causing chaos, use your [commands](#commands) on victim pc

### Autostart
* if you have turned it on you can put your executable whenever you want, it will automaticly copy it self to %temp% and add to autostart

### Commands
* Before all commands should be an ID, default ID is `X` but use it only for `SetID`
* `Cmd <command>` - works like ssh
* `SetCursor <X,Y>` - sets cursor position 
* `BlockCursor <seconds>` - blocks cursor 
* `SetVolume <0-100>` - sets main volume 
* `BlockClipboard <true/false>` - blocks clipboard
* `SetID <ID>` - sets ID
* `msgbox <"Text",number,"Title">` - creates a msgbox 
* `Press <Text to be written>` - writes your text on keyboard 
* `Screenshot`- Makes a screenshot and sends it on your channel
* `SendClipboard` - sends stuff from clipboard to telegram channel
* `Status` - sends some useful info
* `Hotkey` - useful stuff usege of it is only in <ID> help cause im lazy
* `help` - everything you need know about commands

### About Project
 Actually I'm developing it solo but if you want to help me with this half shitty and half stackoverflow code text me on discord (my tag in github profile description), if you have any problems with using this soft text me too
### Disclaimer
Whole this project is only for eductional purposes, I am not responsible for anything you did or will do with this software
