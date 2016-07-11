#ifndef __MAIN_MENU_SCENE_H__
#define __MAIN_MENU_SCENE_H__

#include "PluginAdColony/PluginAdColony.h"

class MainMenuScene : public cocos2d::LayerColor
{
public:
    static cocos2d::Scene* createScene();

    
    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    void goToGameScene(cocos2d::Ref* ref);
    // implement the "static create()" method manually
    CREATE_FUNC(MainMenuScene);
    
     
    
};

#endif // __MAIN_MENU_SCENE_H__
