
#pragma once

class input_class
{

	bool _keys[256];

public:

	input_class();
	~input_class();

	bool initialize();
	void shutdown();

	void set_key_down(unsigned int);
	void set_key_up(unsigned int);
	bool is_key_down(unsigned int);
};
