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
    currentShapePoint = Node::create();
    currentShapePoint->setPosition(Vec2(screenEndX - (WALL_WIDTH),screenEndY - (WALL_WIDTH/2.0)));
    this->addChild(currentShapePoint);
    generationTime =1.0f;
    this->schedule(schedule_selector(GameWorld::shapeGenerator), generationTime);
    this->schedule(schedule_selector(GameWorld::currentShapeChooser),generationTime*4.0f);
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
    
    
    auto shape = getShape();
    auto rotateShapeAction = RepeatForever::create(RotateBy::create(1.0f, 180));
    shape->runAction(rotateShapeAction);
    
    
    rotationPoint->addChild(shape);
    
    WallInfo wall = getInitialEndLocation();
    auto rotationPointAction = MoveTo::create(2, wall.positionOnWall);
    Shape entry;
    entry.rotationPoint = rotationPoint;
    entry.shape = shape;
    entry.shapeType = entry.shape->getTag();
    entry.timeDelay = 0;
    entry.initPosition = Vec2(screenCentreX,screenCentreY);
    entry.wall = wall;
    shapeList.push_back(entry);
    
    rotationPoint->runAction(Sequence::create(rotationPointAction,CallFunc::create(CC_CALLBACK_0(GameWorld::wallHit,this,rotationPoint,entry)),NULL));
    
    this->addChild(rotationPoint,1);
    
  //  int direction =random(0,DIRECTION_COUNT-1);
    
   /* auto rotationPointMoveAction =  RepeatForever::create(
                                                          MoveBy::create(
                                                                         obstacleTime, Vec2(
                                                                                            shapeDirections[direction][0]*DIRECTION_SPEED,              shapeDirections[direction][1]*DIRECTION_SPEED)));
    */
   // rotationPointMoveAction->setTag(ROTATIONPOINT_ACTION_TAG);
   // rotationPoint->runAction(rotationPointMoveAction);
    
    
   
    
    
    
   // shapeList.insert(0, entry);
}

void GameWorld::currentShapeChooser(float dt)
{
    auto currentShape = getShape();
    currentShape->setScale((CIRCLE_MARGIN)/((float)WALL_WIDTH)/1.5f);
    currentShapePoint->removeAllChildren();
    currentShapePoint->addChild(currentShape);
    
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

GameWorld::WallInfo GameWorld::getInitialEndLocation()
{
    int chosenWall =random(TOP_WALL, RIGHT_WALL);
    WallInfo wall;
    
    /*wall.wallType = RIGHT_WALL;
    int position = random(origin.y +WALL_WIDTH+CIRCLE_MARGIN,screenEndY- WALL_WIDTH - CIRCLE_MARGIN);
    wall.positionOnWall =  Vec2(screenEndX-WALL_WIDTH-CIRCLE_MARGIN,position);
    return wall;*/
    if(chosenWall == TOP_WALL)
    {
        wall.wallType = TOP_WALL;
        int position = random(origin.x +WALL_WIDTH+CIRCLE_MARGIN,screenEndX - WALL_WIDTH - CIRCLE_MARGIN);
        wall.positionOnWall =  Vec2(position,screenEndY-WALL_WIDTH-CIRCLE_MARGIN);
    }
    else if(chosenWall == BOTTOM_WALL)
    {
        wall.wallType = BOTTOM_WALL;
        int position = random(origin.x +WALL_WIDTH+CIRCLE_MARGIN,screenEndX - WALL_WIDTH - CIRCLE_MARGIN);
        wall.positionOnWall =  Vec2(position,origin.y+WALL_WIDTH+CIRCLE_MARGIN);
    }
    else if(chosenWall == LEFT_WALL)
    {
        wall.wallType = LEFT_WALL;
        int position = random(origin.y +WALL_WIDTH+CIRCLE_MARGIN,screenEndY- WALL_WIDTH - CIRCLE_MARGIN);
        wall.positionOnWall =  Vec2(origin.x+WALL_WIDTH+CIRCLE_MARGIN,position);
    }
    else
    {
        wall.wallType = RIGHT_WALL;
        int position = random(origin.y +WALL_WIDTH+CIRCLE_MARGIN,screenEndY- WALL_WIDTH - CIRCLE_MARGIN);
        wall.positionOnWall =  Vec2(screenEndX-WALL_WIDTH-CIRCLE_MARGIN,position);
    }
    
    return wall;
}

void GameWorld::wallHit(Node *point,Shape shape)
{
    
    float x1 = shape.initPosition.x;
    float y1 = shape.initPosition.y;
    float x2 = shape.wall.positionOnWall.x;
    float y2 = shape.wall.positionOnWall.y;
    float theta = atan((y2 - y1)/(x2 - x1));
    float slope = tan(M_PI-theta);
    
    shape.initPosition = shape.wall.positionOnWall;
    
    if(shape.wall.wallType == TOP_WALL)
    {
        if(x2 > x1 && y2 > y1)
        {
            Vec2 rightWallPoint = getRightWallCoords(x2, y2, slope);
            if(isPointOnRightWall(rightWallPoint))
            {
                shape.wall.positionOnWall = rightWallPoint;
                shape.wall.wallType = RIGHT_WALL;
            }
            else
            {
                Vec2 bottomWallPoint = getBottomWallCoords(x2, y2, slope);
                shape.wall.positionOnWall = bottomWallPoint;
                shape.wall.wallType = BOTTOM_WALL;
            }
        }
        else if(x2 < x1 && y2>y1)
        {
            Vec2 leftWallPoint = getLeftWallCoords(x2, y2, slope);
            if(isPointOnLeftWall(leftWallPoint))
            {
                shape.wall.positionOnWall = leftWallPoint;
                shape.wall.wallType = LEFT_WALL;
            }
            else
            {
                Vec2 bottomWallPoint = getBottomWallCoords(x2, y2, slope);
                shape.wall.positionOnWall = bottomWallPoint;
                shape.wall.wallType = BOTTOM_WALL;
            }
        }
    }
    else if(shape.wall.wallType == BOTTOM_WALL)
    {
        if (x2 > x1 && y2 < y1)
        {
            Vec2 rightWallPoint = getRightWallCoords(x2, y2, slope);
            if(isPointOnRightWall(rightWallPoint))
            {
                shape.wall.positionOnWall = rightWallPoint;
                shape.wall.wallType = RIGHT_WALL;
            }
            else
            {
                Vec2 topWallPoint = getTopWallCoords(x2, y2, slope);
                shape.wall.positionOnWall = topWallPoint;
                shape.wall.wallType = TOP_WALL;
            }
        }
        else if (x2 < x1 && y2 < y1)
        {
            Vec2 leftWallPoint = getLeftWallCoords(x2, y2, slope);
            if(isPointOnLeftWall(leftWallPoint))
            {
                shape.wall.positionOnWall = leftWallPoint;
                shape.wall.wallType = LEFT_WALL;
            }
            else
            {
                Vec2 topWallPoint = getTopWallCoords(x2, y2, slope);
                shape.wall.positionOnWall = topWallPoint;
                shape.wall.wallType = TOP_WALL;
            }
        }
    }
    else if(shape.wall.wallType == LEFT_WALL)
    {
        if(x2<x1 && y2>y1)
        {
            Vec2 rightWallPoint = getRightWallCoords(x2, y2, slope);
            if(isPointOnRightWall(rightWallPoint))
            {
                shape.wall.positionOnWall = rightWallPoint;
                shape.wall.wallType = RIGHT_WALL;
            }
            else
            {
                Vec2 topWallPoint = getTopWallCoords(x2, y2, slope);
                shape.wall.positionOnWall = topWallPoint;
                shape.wall.wallType = TOP_WALL;
            }
        }
        else if (x2<x1 && y2 < y1)
        {
            Vec2 rightWallPoint = getRightWallCoords(x2, y2, slope);
            if(isPointOnRightWall(rightWallPoint))
            {
                shape.wall.positionOnWall = rightWallPoint;
                shape.wall.wallType = RIGHT_WALL;
            }
            else
            {
                Vec2 bottomWallPoint = getBottomWallCoords(x2, y2, slope);
                shape.wall.positionOnWall = bottomWallPoint;
                shape.wall.wallType = BOTTOM_WALL;
            }
        }
    }
    else if(shape.wall.wallType == RIGHT_WALL)
    {
        if(x2>x1 && y2>y1)
        {
            Vec2 leftWallPoint = getLeftWallCoords(x2, y2, slope);
            if(isPointOnLeftWall(leftWallPoint))
            {
                shape.wall.positionOnWall = leftWallPoint;
                shape.wall.wallType = LEFT_WALL;
            }
            else
            {
                Vec2 topWallPoint = getTopWallCoords(x2, y2, slope);
                shape.wall.positionOnWall = topWallPoint;
                shape.wall.wallType = TOP_WALL;
            }
        }
        else if(x2>x1 && y2<y1)
        {
            Vec2 leftWallPoint = getLeftWallCoords(x2, y2, slope);
            if(isPointOnLeftWall(leftWallPoint))
            {
                shape.wall.positionOnWall = leftWallPoint;
                shape.wall.wallType = LEFT_WALL;
            }
            else
            {
                Vec2 bottomWallPoint = getBottomWallCoords(x2, y2, slope);
                shape.wall.positionOnWall = bottomWallPoint;
                shape.wall.wallType = BOTTOM_WALL;
            }
        }
    }
    
    shape.rotationPoint->runAction(Sequence::create(MoveTo::create(2,shape.wall.positionOnWall),CallFunc::create(CC_CALLBACK_0(GameWorld::wallHit,this,point,shape)),NULL));
}

cocos2d::Vec2 GameWorld::getTopWallCoords(float x1, float y1, float slope)
{
    float x2 = (screenEndY - WALL_WIDTH - CIRCLE_MARGIN - y1)/slope + x1;
    return Vec2(x2,screenEndY - WALL_WIDTH - CIRCLE_MARGIN);
}

cocos2d::Vec2 GameWorld::getBottomWallCoords(float x1, float y1, float slope)
{
    float x2 = (origin.y + WALL_WIDTH + CIRCLE_MARGIN - y1)/slope + x1;
    return Vec2(x2,origin.y + WALL_WIDTH + CIRCLE_MARGIN);
}

cocos2d::Vec2 GameWorld::getLeftWallCoords(float x1, float y1, float slope)
{
    float y2 = slope * (origin.x+WALL_WIDTH+CIRCLE_MARGIN - x1) + y1;
    return Vec2(origin.x+WALL_WIDTH+CIRCLE_MARGIN,y2);
}

cocos2d::Vec2 GameWorld::getRightWallCoords(float x1, float y1, float slope)
{
    float y2 = slope * (screenEndX - WALL_WIDTH - CIRCLE_MARGIN - x1) + y1;
    return Vec2(screenEndX - WALL_WIDTH - CIRCLE_MARGIN,y2);
}

bool GameWorld::isPointOnTopWall(cocos2d::Vec2 point)
{
    if(((int)point.y == (int)(screenEndY - WALL_WIDTH - CIRCLE_MARGIN)) && (((int)point.x > ((int)origin.x + WALL_WIDTH + CIRCLE_MARGIN)) && ((int)point.x < ((int)screenEndX - WALL_WIDTH - CIRCLE_MARGIN))))
            return true;
        else
            return false;
}

bool GameWorld::isPointOnBottomWall(cocos2d::Vec2 point)
{
    if((int)(point.y == (int)(origin.y + WALL_WIDTH + CIRCLE_MARGIN)) && (((int)point.x > ((int)origin.x + WALL_WIDTH + CIRCLE_MARGIN)) && ((int)point.x < ((int)screenEndX - WALL_WIDTH - CIRCLE_MARGIN))))
        return true;
    else
        return false;
}

bool GameWorld::isPointOnLeftWall(cocos2d::Vec2 point)
{
    if((int)point.x == (int)(origin.x + WALL_WIDTH + CIRCLE_MARGIN))
    { if(((int)point.y > (int)(origin.y + WALL_WIDTH + CIRCLE_MARGIN)) && ((int)point.y <(int)(screenEndY - WALL_WIDTH - CIRCLE_MARGIN)))
        return true;
        else
        return false;
    }
    else
        return false;
}

bool GameWorld::isPointOnRightWall(cocos2d::Vec2 point)
{
    if(((int)point.x == (int)(screenEndX - WALL_WIDTH - CIRCLE_MARGIN)) && (((int)point.y > (int)(origin.y + WALL_WIDTH + CIRCLE_MARGIN)) && ((int)point.y < (int)(screenEndY - WALL_WIDTH - CIRCLE_MARGIN))))
        return true;
    else
        return false;
}