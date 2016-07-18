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
bool GameWorld::init()                          //initialize the game
{
    visibleSize = Director::getInstance()->getVisibleSize();          //Calculating standard values in this set
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
    
    sdkbox::PluginAdColony::show("video");
    
    loadMainMenu();
    return true;
 }

void GameWorld::loadMainMenu()             //This will load the main menu as the game screen and main menu hass common areas
{
    isMainMenuScreen = true;
    auto keypadListener = EventListenerKeyboard::create();
    keypadListener->onKeyPressed = CC_CALLBACK_2(GameWorld::onKeyPressed, this);
    keypadListener->onKeyReleased = CC_CALLBACK_2(GameWorld::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keypadListener, this);
    
    auto playField = DrawNode::create();                                            //creating the play field
    playField->drawSolidRect(Vec2(origin.x+WALL_WIDTH,origin.y+WALL_WIDTH), Vec2(screenEndX-WALL_WIDTH,screenEndY-WALL_WIDTH), Color4F::WHITE);//Color4F(247.0/255.0,196.0/255.0,81/255.0, 1) // Setting the dimensions of the play field
    //  playField->drawSolidRect(Vec2(origin.x+WALL_WIDTH,origin.y+WALL_WIDTH), Vec2(screenEndX-WALL_WIDTH,screenEndY-WALL_WIDTH),Color4F(247.0/255.0,196.0/255.0,81/255.0, 1));
    this->addChild(playField);
    
    titlePart = Label::createWithTTF("Tap Color Score", "fonts/Zygoth.ttf", visibleSize.height/20);
    
    titlePart->setPosition(Vec2(screenCentreX,screenEndY - WALL_WIDTH*3));
    this->addChild(titlePart,2);
    
    this->setColor(getRandomColor());
    titlePart->setColor(this->getColor());
    generationTime =1.0f;
    
    this->schedule(schedule_selector(GameWorld::shapeGenerator), generationTime);          //this event is triggered every generationTime interval to generate a new shape at the centre of the screen
    this->schedule(schedule_selector(GameWorld::currentShapeChooser),generationTime*3.0f);    // this event changes the current shape to be changed at equal intervals
    //  this->scheduleUpdate();


    playButton = MenuItemImage::create("res/playButton.png", "res/playButton.png",
                                            CC_CALLBACK_1(GameWorld::onPlayButtonClick, this));
    playButton->setScale(visibleSize.width/playButton->getContentSize().width/2.5);
    playButton->setPosition(Point(visibleSize.width/2+origin.x, visibleSize.height*0.6+origin.y));
    playButton->setColor(this->getColor());
    
    int best = UserDefault::getInstance()->getIntegerForKey("Best", 0);
    std::stringstream bestScoreStream;
    bestScoreStream << best;
    char bestString[20];
    sprintf(bestString,"Best: %d", best);
    bestScore=Label::createWithTTF(bestString, "fonts/arial.ttf", visibleSize.width/15);
    bestScore->setColor(this->getColor());
    bestScore->setPosition(Vec2(screenCentreX,screenCentreY - 3*WALL_WIDTH));
    this->addChild(bestScore);
    
    mainMenu = Menu::create(playButton,NULL);
    mainMenu->setPosition(Point::ZERO);
    
    this->addChild(mainMenu,2);
}

void GameWorld::onPlayButtonClick(cocos2d::Ref *ref)
{
     releaseResources();
    auto listener = EventListenerTouchOneByOne::create();                       //Activating a mouse listener using MVC model
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(GameWorld::onTouchBegan, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    loadGame();
}

void GameWorld::loadGame()
{
    
    isMainMenuScreen = false;
    score = Label::createWithTTF("0", "fonts/MarkerFelt2.ttf", visibleSize.width/20.0f);
    score->setPosition(Vec2(screenCentreX,screenEndY - (WALL_WIDTH/2.0)));
    this->addChild(score);
    this->schedule(schedule_selector(GameWorld::shapeGenerator), generationTime);          //this event is triggered every generationTime interval to generate a new shape at the centre of the screen
    this->schedule(schedule_selector(GameWorld::currentShapeChooser),generationTime*3.0f);    // this event changes the current shape to be changed at equal intervals
    //  this->scheduleUpdate();
}

void GameWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

Color3B GameWorld::getRandomColor()
{
    int colorValue =random(COLOR_RED,COLOR_ORANGE);
    
    if(colorValue == COLOR_RED)
        return Color3B::RED;
    else if(colorValue == COLOR_GREEN)
        return Color3B::GREEN;
    else if(colorValue == COLOR_BLUE)
        return Color3B::BLUE;
    else
        return Color3B::ORANGE;
}

void GameWorld::shapeGenerator(float dt)   //generates shapes
{
    
    auto rotationPoint = Node::create();               //node to hang shape
    rotationPoint->setPosition(screenCentreX,screenCentreY);
    
    
    auto shape = getShape();              //get a new shape
    auto rotateShapeAction = RepeatForever::create(RotateBy::create(1.0f, 180));  //action to be removed probably
    shape->runAction(rotateShapeAction);
    
    
    rotationPoint->addChild(shape);
    
    WallInfo wall = getInitialEndLocation();          //get the location where the shape should initially go i.e, a point on the wall
    float unitTime = calculateUnitTimeFromDistance(calculateDistance(Vec2(screenCentreX,screenCentreY),wall.positionOnWall));
    auto rotationPointAction = MoveTo::create(unitTime, wall.positionOnWall);     //action to move the shape by moving the parent rotationPoint
    Shape entry;
    entry.rotationPoint = rotationPoint;   //making an entry for std map dictionary
    //entry.shape = shape;
    entry.shapeType = shape->getTag();
    //entry.timeDelay = 0;
    entry.initPosition = Vec2(screenCentreX,screenCentreY);
    entry.wall = wall;
    
    std::string color = shape->getName();          //providing color information to entry
    if(color == "Red")
    {
        entry.color = Color4F::RED;
        entry.colorName = "Red";
    }
    else if(color == "Green")
    {
        entry.color = Color4F::GREEN;
        entry.colorName = "Green";
    }
    else if(color == "Blue")
    {
        entry.color = Color4F::BLUE;
        entry.colorName = "Blue";
    }
    else if(color == "Orange")
    {
        entry.color = Color4F::ORANGE;
        entry.colorName = "Orange";
    }
    shapeList[rotationPoint]=entry;
    
    rotationPoint->runAction(Sequence::create(rotationPointAction,CallFunc::create(CC_CALLBACK_0(GameWorld::wallHit,this,rotationPoint,entry)),NULL));   //This command runs the move action and then calls a function called wallHit when the move action is over. The wall hit calculates the new reflected coordinate and reflects it on to another surface using the concept of linear equations and coordinate geometry.
    
    this->addChild(rotationPoint,1);
    

    
    
   // shapeList.insert(0, entry);
}

void GameWorld::currentShapeChooser(float dt)
{
    /*auto currentShape = getShape();
    currentShape->setScale((CIRCLE_MARGIN)/((float)WALL_WIDTH)/1.5f);
    currentShapePoint->removeAllChildren();
    currentShapePoint->addChild(currentShape);*/
    this->setColor(getRandomColor());
    if(titlePart!=NULL)
    titlePart->setColor(this->getColor());
    if(bestScore!=NULL)
    bestScore->setColor(this->getColor());
    if(playButton!=NULL)
    playButton->setColor(this->getColor());
}

DrawNode* GameWorld::getShape()              //returns a random shape
{
    
    int colorValue =random(COLOR_RED,COLOR_ORANGE);
    std::string colorName;
    
    Color4F color;
    if(colorValue == COLOR_GREEN)
    {
        color = Color4F::GREEN;
        colorName = "Green";
    }
    else if(colorValue == COLOR_BLUE)
    {
         color = Color4F::BLUE;
         colorName = "Blue";
    }
    else if(colorValue == COLOR_RED)
    {
        color = Color4F::RED;
        colorName = "Red";
    }
    else if(colorValue == COLOR_ORANGE)
    {
        color = Color4F::ORANGE;
        colorName = "Orange";
    }
    
    
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
    shape->setName(colorName);
    shape->setTag(CIRCLE_SHAPE);
    return shape;

    
}


void GameWorld::update(float dt)
{
    
}

GameWorld::WallInfo GameWorld::getInitialEndLocation()      //calculates the initial end location for a shape when it is generated
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

void GameWorld::wallHit(Node *point,Shape &shape)
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
    
    shape.color = Color4F(shape.color.r,shape.color.g,shape.color.b,shape.color.a/1.2);
    
   // if(shape.color.a<20)
   // {
        
     //   return;
    //}
    DrawNode *nextStageShape = DrawNode::create();
    nextStageShape->drawDot(Vec2(0,0), CIRCLE_MARGIN, shape.color);
    nextStageShape->setName(shape.colorName);
    nextStageShape->setTag(CIRCLE_SHAPE);
    //shape.shape->removeFromParent();
    //shape.shape = nextStageShape;
    //shape.rotationPoint->addChild(shape.shape);
    //shape.shape->drawDot(Vec2(0,0), CIRCLE_MARGIN, shape.color);
    shape.rotationPoint->getChildren().at(0)->removeFromParent();
    shape.rotationPoint->addChild(nextStageShape);
    float unitTime = calculateUnitTimeFromDistance(calculateDistance(shape.initPosition,shape.wall.positionOnWall));
    shapeList[shape.rotationPoint].color = shape.color;
    shape.rotationPoint->runAction(Sequence::create(MoveTo::create(unitTime,shape.wall.positionOnWall),CallFunc::create(CC_CALLBACK_0(GameWorld::wallHit,this,point,shape)),NULL));
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

float GameWorld::calculateDistance(cocos2d::Vec2 point1, cocos2d::Vec2 point2)
{
    return sqrt(pow((point2.x - point1.x),2) + pow((point2.y - point1.y),2));
}

float GameWorld::calculateUnitTimeFromDistance(float distance)
{
    return (1/(visibleSize.width/2)*distance);
}

bool GameWorld::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    Vec2 mousePoint = touch->getLocation();
    long count = shapeList.size();
    
   // typedef std::map<Node *,Shape>::iterator it_type;
    for(auto iterator=shapeList.cbegin();iterator != shapeList.cend();iterator++)
    {
        Shape shape = iterator->second;
        float circleValue = pow(mousePoint.x - shape.rotationPoint->getPosition().x,2) + pow(mousePoint.y - shape.rotationPoint->getPosition().y,2) - pow(CIRCLE_MARGIN,2);
        
        if(circleValue <= 0)
        {
            //DrawNode *polygon = (DrawNode *)currentShapePoint->getChildren().at(0);
            //int polygonTag = polygon->getTag();
           // if(polygonTag == CIRCLE_SHAPE)
           // {
                //std::string polygonColor = polygon->getName();
                //std::string shapeColor = shape.colorName;
                if(this->getColor() == Color3B(shape.color))
                {
                    shape.rotationPoint->stopAllActions();
                    shape.rotationPoint->removeAllChildren();
                    shape.rotationPoint->removeFromParent();
                    shapeList.erase(iterator++);
                    //CCLOG("Alpha=%f",shape.color.a);
                    scoreValue +=  1.0/shape.color.a *5.0;
                    std::stringstream scoreStringStream;
                    scoreStringStream << scoreValue;
                    std::string scoreString = scoreStringStream.str();
                    score->setString(scoreString);
                    break;
                }
                else
                {
                    releaseResources();
                    int best = UserDefault::getInstance()->getIntegerForKey("Best", 0);
                    if(scoreValue > best)
                        UserDefault::getInstance()->setIntegerForKey("Best", scoreValue);
                    loadMainMenu();
                   // loadGameEndedScreen();
                    //int bestScore = UserDefault::getInstance()->getIntegerForKey("Best", 0);
                    break;
                }
            //}
            
        }
    }
    return true;
}



void GameWorld::releaseResources()
{
    //long count =shapeList.size();
    for(auto iterator=shapeList.cbegin();iterator != shapeList.cend();iterator++)
    {
        Shape shape = iterator->second;
        shape.rotationPoint->removeAllChildren();
        shape.rotationPoint->removeFromParent();
        
    }
    
    shapeList.erase(shapeList.begin(),shapeList.end());

    if(score != NULL){
        score->removeFromParent();
        score = NULL;
    }
        if(mainMenu !=NULL)
    {
        mainMenu->removeFromParent();
        mainMenu = NULL;
    }
    
    if(titlePart !=NULL)
    {
        titlePart->removeFromParent();
        titlePart = NULL;
    }
    
    if(bestScore != NULL)
    {
        bestScore->removeFromParent();
        bestScore = NULL;
    }
    
    if(playButton !=NULL)
    {
        playButton->removeFromParent();
        playButton = NULL;
    }
    
    this->unscheduleAllCallbacks();
    //if(Director::getInstance()->getTextureCache() !=NULL)
    Director::getInstance()->getTextureCache()->removeUnusedTextures();
    //loadGame();
    
}

void GameWorld::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event)
{
    if (keyCode == EventKeyboard::KeyCode::KEY_BACK)
    {
       if(isMainMenuScreen)
        Director::getInstance()->end();
    }
}