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
    cocos2d::Node* currentShapePoint;
    int screenCentreX;
    int screenCentreY;
    int screenEndX;
    int screenEndY;
    
    struct WallInfo
    {
        int wallType;
        cocos2d::Vec2 positionOnWall;
    };
    
    struct Shape
    {
        Node* rotationPoint;
       // cocos2d::DrawNode* shape;
        cocos2d::Color4F color;
        std::string colorName;
        int shapeType;
        cocos2d::Vec2 initPosition;
        struct WallInfo wall;
        
    };
   // std::vector<Shape> shapeList;
    std::map<Node*,Shape> shapeList;
    virtual bool init();
    void loadScene();
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    void genrateShapes();
    
    // implement the "static create()" method manually
    CREATE_FUNC(GameWorld);
    cocos2d::DrawNode* getShape();
    void shapeGenerator(float dt);
    void currentShapeChooser(float dt);
    void update(float dt);
    WallInfo getInitialEndLocation();
    void wallHit(cocos2d::Node *point,Shape &shape);
    cocos2d::Vec2 getTopWallCoords(float x1,float y1,float slope);
    cocos2d::Vec2 getBottomWallCoords(float x1,float y1,float slope);
    cocos2d::Vec2 getLeftWallCoords(float x1,float y1,float slope);
    cocos2d::Vec2 getRightWallCoords(float x1,float y1,float slope);
    bool isPointOnTopWall(cocos2d::Vec2 point);
    bool isPointOnBottomWall(cocos2d::Vec2 point);
    bool isPointOnLeftWall(cocos2d::Vec2 point);
    bool isPointOnRightWall(cocos2d::Vec2 point);
    float calculateDistance(cocos2d::Vec2 point1,cocos2d::Vec2 point2);
    float calculateUnitTimeFromDistance(float distance);
    bool onTouchBegan(cocos2d::Touch *touch,cocos2d::Event *event);
    cocos2d::Color3B getRandomColor();
    void releaseResources();
};

#endif // __HELLOWORLD_SCENE_H__
