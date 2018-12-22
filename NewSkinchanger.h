





class Weapon_t
{
public:
	bool ChangerEnabled = false;
	int ChangerSkin = 0;
	char ChangerName[32] = "";
	int ChangerStatTrak = 0;
	int ChangerSeed = 0;
	float ChangerWear = 0;
};
extern Weapon_t* weapons;

extern void newskinchanger();
