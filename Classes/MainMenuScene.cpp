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
    
    
    
    
    for(int i=0; i<10;i++)
    {
        Color4F color = getRandomColor();
       
        float xCoordinate = random(origin.x + WALL_WIDTH + CIRCLE_MARGIN,
                                   origin.x + visibleSize.width - WALL_WIDTH - CIRCLE_MARGIN);
        float yCoordinate = random(origin.y + WALL_WIDTH + CIRCLE_MARGIN,
                                   origin.y + visibleSize.height - WALL_WIDTH - CIRCLE_MARGIN);
        
        //CCLOG("Scale = %f",playButton->getScale());
       // float scale = playButton->getScale();
       /* if(
           (xCoordinate >= (origin.x + visibleSize.width/2 - playButton->getContentSize().width/2*scale) &&
            xCoordinate <= (origin.x + visibleSize.width/2 + playButton->getContentSize().width/2*scale))
           &&
           ((yCoordinate >= (origin.y + visibleSize.height/2 - playButton->getContentSize().height/2*scale)) &&
            (yCoordinate <= (origin.y + visibleSize.height/2 + playButton->getContentSize().height/2*scale))
            ))
        {
            i--;
            continue;
        }*/
        
         DrawNode *shape = DrawNode::create();
        shape->drawDot(Vec2(xCoordinate,yCoordinate), CIRCLE_MARGIN, Color4F(color.r,color.g,color.b,color.a/2.0));
        
        float xDisplacement = random(1,10);
        float yDisplacement = random(1,10);
    
        shape->runAction(MoveBy::create(1.0f, Vec2(xDisplacement,yDisplacement)));
        this->addChild(shape);
    }
    
    this->addChild(menu);
    return true;
}

void MainMenuScene::goToGameScene(cocos2d::Ref *ref)
{
    auto scene = GameWorld::createScene();
    Director::getInstance()->pushScene(TransitionFade::create(TRANSITION_TIME, scene));
    //  sdkbox::PluginReview::show();
   // sdkbox::PluginAdColony::show("video");

    //auto scene = GameWorld::createScene();
    //Director::getInstance()->pushScene(TransitionFade::create(TRANSITION_TIME, scene));
}

Color4F MainMenuScene::getRandomColor()
{
    int colorValue =random(COLOR_RED,COLOR_ORANGE);
    
    if(colorValue == COLOR_RED)
        return Color4F::RED;
    else if(colorValue == COLOR_GREEN)
        return Color4F::GREEN;
    else if(colorValue == COLOR_BLUE)
        return Color4F::BLUE;
    else
        return Color4F::ORANGE;
}
