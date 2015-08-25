#pragma once


class Settings;


class Application {
	Application(const Application&) = delete;
	void operator=(const Application&) = delete;
public:
	Application(int, char**);
	~Application();
	int run(void);
private:
	void doRun(void);

	Settings* settings;
};
