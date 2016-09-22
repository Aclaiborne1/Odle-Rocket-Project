#ifndef RocketH
#define RocketH

class Rocket
{
  public:
	void siren();
	void initialize();
	int systemtest();
	float gettemperature();
	float getgroundLevelPressure();
	int getfeet();
	int getXgees();
        int getYgees();
        int getZgees();
        float getvelocity(float tt, int ss);
};

#endif
