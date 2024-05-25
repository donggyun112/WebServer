#include "Manager.hpp"

ResponseUtils Manager::responseUtils;
Utils Manager::utils;
RequestUtils Manager::requestUtils;
Config Manager::config;

Manager::Manager() {
}

Manager::~Manager() {
}

void Manager::initConfig(Config &Conf) {
	config = Conf;
}