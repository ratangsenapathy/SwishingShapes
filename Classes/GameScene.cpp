#include "GameScene.h"

USING_NS_CC;

Scene* GameWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = GameWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameWorld::init()
{
    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
    screenCentreX = origin.x + visibleSize.width/2;
    screenCentreY = origin.y + visibleSize.height/2;
    screenEndX = origin.x + visibleSize.width;
    screenEndY = origin.y + visibleSize.height;
    
    if( !LayerColor::initWithColor(Color4B::GRAY))
    {
        return false;
    }
    
    obstacleTime = 0.5f;
    
    
    auto playField = DrawNode::create();
    playField->drawSolidRect(Vec2(origin.x+WALL_WIDTH,origin.y+WALL_WIDTH), Vec2(screenEndX-WALL_WIDTH,screenEndY-WALL_WIDTH), Color4F(247.0/255.0,196.0/255.0,81/255.0, 1));
  
    this->addChild(playField);
    
    generationTime =0.5f;
    this->schedule(schedule_selector(GameWorld::shapeGenerator), generationTime);
    
  //  this->scheduleUpdate();
   
    return true;
}


void GameWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void GameWorld::shapeGenerator(float dt)
{
    
    auto rotationPoint = Node::create();
    rotationPoint->setPosition(screenCentreX,screenCentreY);
    
    
    auto shape =getShape();
    auto rotateShapeAction = RepeatForever::create(RotateBy::create(1.0f, 180));
    shape->runAction(rotateShapeAction);
    
    
    rotationPoint->addChild(shape);
    
    Vec2 destination = getEndLocation();
    auto rotationPointAction = MoveTo::create(2, destination);
    rotationPoint->runAction(rotationPointAction);
    
    this->addChild(rotationPoint,1);

  //  int direction =random(0,DIRECTION_COUNT-1);
    
   /* auto rotationPointMoveAction =  RepeatForever::create(
                                                          MoveBy::create(
                                                                         obstacleTime, Vec2(
                                                                                            shapeDirections[direction][0]*DIRECTION_SPEED,              shapeDirections[direction][1]*DIRECTION_SPEED)));
    */
   // rotationPointMoveAction->setTag(ROTATIONPOINT_ACTION_TAG);
   // rotationPoint->runAction(rotationPointMoveAction);
    
    
   
    Shape entry;
    entry.rotationPoint = rotationPoint;
    entry.shape = shape;
    entry.shapeType = entry.shape->getTag();
    entry.timeDelay = 0;
    entry.initPosition = Vec2(screenCentreX,screenCentreY);
    entry.finalPosition = destination;
    shapeList.push_back(entry);
    
    
   // shapeList.insert(0, entry);
}

DrawNode* GameWorld::getShape()
{
    
    int shapeType = random(1 , CIRCLE_SHAPE);
    int colorValue =random(COLOR_RED,COLOR_ORANGE);
    
    
    Color4F color;
    if(colorValue == COLOR_GREEN)
        color = Color4F::GREEN;
    else if(colorValue == COLOR_BLUE)
        color = Color4F::BLUE;
    else if(colorValue == COLOR_RED)
        color = Color4F::RED;
    else if(colorValue == COLOR_ORANGE)
        color = Color4F::ORANGE;
    
    
    DrawNode *shape = DrawNode::create();
   /* if(shapeType <= TRIANGLE_SHAPE)
    {
        shape->drawTriangle(Vec2(TRIANGLE_MARGIN,TRIANGLE_MARGIN),
                            Vec2(-0.5*TRIANGLE_MARGIN-sqrt(3)/2.0*TRIANGLE_MARGIN,sqrt(3)/2.0*TRIANGLE_MARGIN - 0.5*TRIANGLE_MARGIN),
                            Vec2(-0.5*TRIANGLE_MARGIN+sqrt(3)/2.0*TRIANGLE_MARGIN,-sqrt(3)/2.0*TRIANGLE_MARGIN-0.5*TRIANGLE_MARGIN),
                            color);
        shape->setTag(TRIANGLE_SHAPE);
        
       // shape->drawTriangle(Vec2(0,TRIANGLE_MARGIN*1.5),
       //                     Vec2(TRIANGLE_MARGIN*0.75,-TRIANGLE_MARGIN*0.75),
       //                     Vec2(-TRIANGLE_MARGIN*0.75,-TRIANGLE_MARGIN*0.75),
       //                     color);
    }
    else if(shapeType <= SQUARE_SHAPE)
    {
        shape->drawSolidRect(Vec2(-SQUARE_MARGIN,SQUARE_MARGIN),
                             Vec2(SQUARE_MARGIN,-SQUARE_MARGIN),
                             color);
        shape->setTag(SQUARE_SHAPE);
    }
    else if(shapeType <= CIRCLE_SHAPE)
    {
        shape->drawDot(Vec2(0,0),
                       CIRCLE_MARGIN,
                       color);
        shape->setTag(CIRCLE_SHAPE);
    }*/
    
    shape->drawDot(Vec2(0,0),
                   CIRCLE_MARGIN,
                   color);
    shape->setTag(CIRCLE_SHAPE);
    return shape;

    
}


void GameWorld::update(float dt)
{
    CCLOG("DT=%f",dt);
    unsigned long shapeCount =shapeList.size();
    
    for(unsigned long i=0;i<shapeCount;i++)
    {
        auto entry = shapeList.at(i);
        float xCoordinate = entry.rotationPoint->getPosition().x;
        float yCoordinate = entry.rotationPoint->getPosition().y;
        
        if (entry.shapeType == CIRCLE_SHAPE)
        {
            if(xCoordinate - CIRCLE_MARGIN <=(origin.x+WALL_WIDTH) || (xCoordinate + CIRCLE_MARGIN >= screenEndX - WALL_WIDTH) || yCoordinate - CIRCLE_MARGIN <=(origin.y+WALL_WIDTH) || (yCoordinate + CIRCLE_MARGIN >= screenEndY - WALL_WIDTH))
            {
                if(entry.timeDelay == 0.0f)
                { entry.rotationPoint->stopActionByTag(ROTATIONPOINT_ACTION_TAG);
                auto moveAction = MoveTo::create(3,Vec2(screenCentreX,screenCentreY));
                //moveAction->setTag(ROTATIONPOINT_ACTION_TAG);
                entry.rotationPoint->runAction(moveAction);
                    entry.timeDelay = 0.5f;
                }
                else
                {
                    entry.timeDelay -=dt;
                    if (entry.timeDelay<=0.0f)
                    {
                        entry.timeDelay = 0.0f;
                    }
                }
                
                
            }
        }
        
    }
   
}

cocos2d::Vec2 GameWorld::getEndLocation()
{
    int chosenWall =random(TOP_WALL, RIGHT_WALL);
    if(chosenWall == TOP_WALL)
    {
        int position = random(origin.x +WALL_WIDTH+CIRCLE_MARGIN,screenEndX - WALL_WIDTH - CIRCLE_MARGIN);
        return Vec2(position,screenEndY-WALL_WIDTH-CIRCLE_MARGIN);
    }
    else if(chosenWall == BOTTOM_WALL)
    {
        int position = random(origin.x +WALL_WIDTH+CIRCLE_MARGIN,screenEndX - WALL_WIDTH - CIRCLE_MARGIN);
        return Vec2(position,origin.y+WALL_WIDTH+CIRCLE_MARGIN);
    }
    else if(chosenWall == LEFT_WALL)
    {
        int position = random(origin.y +WALL_WIDTH+CIRCLE_MARGIN,screenEndY- WALL_WIDTH - CIRCLE_MARGIN);
        return Vec2(origin.x+WALL_WIDTH+CIRCLE_MARGIN,position);
    }
    else
    {
        int position = random(origin.y +WALL_WIDTH+CIRCLE_MARGIN,screenEndY- WALL_WIDTH - CIRCLE_MARGIN);
        return Vec2(screenEndX-WALL_WIDTH-CIRCLE_MARGIN,position);
    }
}
