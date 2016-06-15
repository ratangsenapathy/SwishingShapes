#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "Definitions.h"

class GameWorld : public cocos2d::LayerColor
{
public:
    static cocos2d::Scene* createScene();
    float shapeDirections[DIRECTION_COUNT][2] = {SHAPE_DIRECTIONS};
    float obstacleTime;
    float generationTime;
    cocos2d::Size visibleSize;
    cocos2d::Vec2 origin;
    int screenCentreX;
    int screenCentreY;
    int screenEndX;
    int screenEndY;
    
    struct Shape
    {
        Node* rotationPoint;
        cocos2d::DrawNode* shape;
        int shapeType;
        float timeDelay;
        cocos2d::Vec2 initPosition;
        cocos2d::Vec2 finalPosition;
    };
    std::vector<Shape> shapeList;
    
    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    void genrateShapes();
    
    // implement the "static create()" method manually
    CREATE_FUNC(GameWorld);
    cocos2d::DrawNode* getShape();
    void shapeGenerator(float dt);
    void update(float dt);
    cocos2d::Vec2 getEndLocation();
};

#endif // __HELLOWORLD_SCENE_H__
