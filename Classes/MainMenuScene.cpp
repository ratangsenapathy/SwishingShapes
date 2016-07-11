#include "MainMenuScene.h"
#include "Definitions.h"
#include "GameScene.h"

USING_NS_CC;


Scene* MainMenuScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = MainMenuScene::create();
    

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MainMenuScene::init()
{    
    //////////////////////////////
    // 1. super init first
    if( !LayerColor::initWithColor(Color4B::WHITE))
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    auto playButton = MenuItemImage::create("res/play.png", "res/play_clicked.png",
                                            CC_CALLBACK_1(MainMenuScene::goToGameScene, this));
    playButton->setScale(visibleSize.width/playButton->getContentSize().width/2.5);
    playButton->setPosition(Point(visibleSize.width/2+origin.x, visibleSize.height*0.6+origin.y));
    
    
    auto menu = Menu::create(playButton,NULL);
    menu->setPosition(Point::ZERO);
    
    this->addChild(menu);
    
    return true;
}

void MainMenuScene::goToGameScene(cocos2d::Ref *ref)
{
    sdkbox::PluginAdColony::show("image");

    //auto scene = GameWorld::createScene();
    //Director::getInstance()->pushScene(TransitionFade::create(TRANSITION_TIME, scene));
}

