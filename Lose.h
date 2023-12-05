#include "Scene.h"

class Lose : public Scene {
public:
    int ENEMY_COUNT = 0;
    
    ~Lose();
    
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};

