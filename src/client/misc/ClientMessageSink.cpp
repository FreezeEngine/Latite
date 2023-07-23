#include "ClientMessageSink.h"
#include "util/Logger.h"
#include "sdk/common/client/player/LocalPlayer.h"
#include "sdk/common/client/game/ClientInstance.h"
#include "pch.h"

void ClientMessageSink::doPrint(int numMessages) {
	int msgs = 0;
	for (auto it = messages.begin(); it != messages.end();) {
		if (msgs >= numMessages) break;

		auto& msg = *it;
		auto cInst = sdk::ClientInstance::get();
		auto lp = cInst->getLocalPlayer();
		if (lp) {
			lp->displayClientMessage(msg);
			messages.erase(it);
			continue;
		}
		++it;
		msgs++;
	}
}

void ClientMessageSink::push(std::string const& message) {
	this->sinkLock.lock();
	this->messages.push_back(message);
	this->sinkLock.unlock();
}

void ClientMessageSink::display(std::string const& message) {
	auto cInst = sdk::ClientInstance::get();
	auto lp = cInst->getLocalPlayer();
	if (lp) {
		lp->displayClientMessage(message);
	}
}
