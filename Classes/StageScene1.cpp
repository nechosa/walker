#include "StageScene1.h"

USING_NS_CC;

Scene* StageScene1::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    auto hudLayer = Layer::create();
    scene->addChild(hudLayer, 2);

    auto backgroundLayer = ParallaxScrollNode::create();
    // 'layer' is an autorelease object
    scene->addChild(backgroundLayer, 0);

     auto layer = StageScene1::createWithHUDAndBackGround(hudLayer, backgroundLayer);

    scene->addChild(layer, 1);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool StageScene1::init()
{
     CCLOG("StageScene1::init()");
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    worldSize = Size(1000, visibleSize.height);

    auto label = Label::createWithTTF("Hello Scene2", "fonts/Marker Felt.ttf", 24);

    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

     hudLayer->addChild(label, 1);

    // add "HelloWorld" splash screen"
     auto sprite = Sprite::create("BackgroundGame.png");
     Sprite * backgroundSprite1 = Sprite::create("BackgroundGame.png");
     Sprite * backgroundSprite2 = Sprite::create("BackgroundGame.png");
     //backgroundLayer->addInfiniteScrollXWithZ(0, Point(1.0f, 1.0f), Point(0,0), backgroundSprite1, NULL);

      sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
      this->addChild(sprite, 0);

     // add joystick
     int joystickOffset = 10;

     Rect joystickBaseDimensions = Rect(0, 0, 64.0f, 64.0f);
     Point joystickBasePosition  = Point(origin.x + (joystickBaseDimensions.size.width / 2) + joystickOffset, origin.y + (joystickBaseDimensions.size.height / 2) + joystickOffset);

     Rect accelButtonDimensions = Rect(0, 0, 64.0f, 64.0f);
     Point accelButtonPosition  = Point(origin.x + visibleSize.width - (joystickBaseDimensions.size.width / 2) - joystickOffset,origin.y + (joystickBaseDimensions.size.height / 2) + joystickOffset);

     // add joystick base
     SneakyJoystickSkinnedBase *joystickBase = SneakyJoystickSkinnedBase::create();
     SneakyJoystick *joystick = new SneakyJoystick();
     joystick->initWithRect(joystickBaseDimensions);
     joystickBase->setBackgroundSprite(CCSprite::create("joystick/dpadDown.png"));
     joystickBase->setThumbSprite(CCSprite::create("joystick/joystickDown.png"));
     joystickBase->setJoystick(joystick);
     joystickBase->setPosition(joystickBasePosition);
     leftJoystick = joystickBase->getJoystick();
     hudLayer->addChild(joystickBase);

     // add joystick button
     SneakyButtonSkinnedBase *action1ButtonBase = SneakyButtonSkinnedBase::create();
     SneakyButton *button1 = new SneakyButton();
     button1->initWithRect(accelButtonDimensions);
     action1ButtonBase->setDefaultSprite(CCSprite::create("joystick/action1Down.png"));
     action1ButtonBase->setActivatedSprite(CCSprite::create("joystick/action1Down.png"));
     action1ButtonBase->setPressSprite(CCSprite::create("joystick/action1Up.png"));
     action1ButtonBase->setButton(button1);
     action1ButtonBase->setPosition(accelButtonPosition);
     action1Button = action1ButtonBase->getButton();
     hudLayer->addChild(action1ButtonBase);

     // create physics
     createPhysics();

     // add the player
     player = new Player();
     player->init(this, world);
     addChild(player->getBatchNode());

     // camera follow the player
     runAction(Follow::create(player->getSprite(), Rect(0, 0, backgroundSprite1->getContentSize().width, backgroundSprite1->getContentSize().height)));

     // rest of framework init process
     scheduleUpdate();

    return true;
}


void StageScene1::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}

StageScene1 *StageScene1::createWithHUDAndBackGround(Layer *hudLayer, ParallaxScrollNode *backgroundLayer)
{
     CCLOG("StageScene1 *StageScene1");
    auto object = new StageScene1();
    object->setHUDLayer(hudLayer);
    object->setBackgroundLayer(backgroundLayer);

    if (object && object->init())
        object->autorelease();
     CCLOG("return object");
     return object;
//    else
//    {
//        delete object;
//        object = NULL;
//        return NULL;
//    }
}

void StageScene1::update(float dt)
{
    // update the physics
    updatePhysics(dt);

    // update the player
    if (player)
    {
        player->update(dt);
        player->updateVelocity(leftJoystick->getVelocity());

        if (action1Button->getIsActive())
        {
            player->actionButtonPressed(1);
        }
    }

    // update collisions
    std::vector<ContactData>::iterator pos;
    for(pos = contactListener->_contacts.begin(); pos != contactListener->_contacts.end(); ++pos)
    {
        ContactData contact = *pos;

        // get the box2d bodies for each object
        b2Body *bodyA = contact.fixtureA->GetBody();
        b2Body *bodyB = contact.fixtureB->GetBody();

        if (bodyA->GetUserData() != NULL && bodyB->GetUserData() != NULL)
        {
            Entity *entityA = (Entity*)bodyA->GetUserData();
            Entity *entityB = (Entity*)bodyB->GetUserData();

            int iTagA = entityA->getTag();
            int iTagB = entityB->getTag();

            if (iTagA == Entity::TAG_PLAYER)
            {

            }
            else if (iTagB == Entity::TAG_PLAYER)
            {

            }
        }
    }

    // update the background parallax
    //backgroundLayer->updateWithVelocity(Point(-2.0f, 0), dt);
    //backgroundLayer->updateWithXPosition(-player->getSprite()->getPosition().x, dt);
}

void StageScene1::updatePhysics(float dt)
{
    world->Step(dt, 10, 10);
}

void StageScene1::createPhysics()
{
    // world
    b2Vec2 gravity = b2Vec2(0.0f, -10.0f);
    world = new b2World(gravity);

    // contact listener
    contactListener = new ContactListener();
    world->SetContactListener(contactListener);

    // create collision wall and ground
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0, (66 / PTM_RATIO));
    b2Body *_groundBody;
    _groundBody = world->CreateBody(&groundBodyDef);

    b2EdgeShape groundBox;
    b2FixtureDef groundBoxDef;
    groundBoxDef.shape = &groundBox;

    b2Fixture *_bottomFixture;

    groundBox.Set(b2Vec2(0,0), b2Vec2(worldSize.width/PTM_RATIO, 0));
    _bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

    groundBox.Set(b2Vec2(0,0), b2Vec2(0, worldSize.height/PTM_RATIO));
    _groundBody->CreateFixture(&groundBoxDef);

    groundBox.Set(b2Vec2(0, worldSize.height/PTM_RATIO), b2Vec2(worldSize.width/PTM_RATIO,
                                                              worldSize.height/PTM_RATIO));
    _groundBody->CreateFixture(&groundBoxDef);

    groundBox.Set(b2Vec2(worldSize.width/PTM_RATIO, worldSize.height/PTM_RATIO),
                  b2Vec2(worldSize.width/PTM_RATIO, 0));
    _groundBody->CreateFixture(&groundBoxDef);
}


void StageScene1::setHUDLayer(Layer *layer)
{
    hudLayer = layer;
}

void StageScene1::setBackgroundLayer(ParallaxScrollNode *layer)
{
    backgroundLayer = layer;
}
