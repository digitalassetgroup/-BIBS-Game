Cocos2dGAFPlayer
================

Play GAF format on iOS via cocos2d

Howto:
-----------------------
   * Menu: File -> Add to -> select GAFPlayer.xcodeproj;
   * Menu: File -> Add to -> select cocos2d-ios.xcodeproj;
   * Header Search Paths: path to GAFPlayer/Library/Sources and cocos2d/cocos2d folders;
   * Other Linker Flags: -ObjC -lz;
   * add a shaders folder to the project.
   
see Examples/HelloChris projecr
   
Download from Github
--------------------

    $ git clone git://github.com/CatalystApps/Cocos2dGAFPlayer.git
    $ cd Cocos2dGAFPlayer
    $ git checkout master
    $ git submodule update --init
    $ cd Externals/cocos2d
    $ git checkout develop-v3
    $ git submodule update --init
