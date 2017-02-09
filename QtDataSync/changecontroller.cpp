#include "changecontroller.h"
using namespace QtDataSync;

ChangeController::ChangeController(DataMerger *merger, QObject *parent) :
	QObject(parent),
	merger(merger)
{
	merger->setParent(this);
}

void ChangeController::initialize()
{
	merger->initialize();
}

void ChangeController::finalize()
{
	merger->finalize();
}

void ChangeController::setInitialLocalStatus(const StateHolder::ChangeHash &changes)
{
	for(auto it = changes.constBegin(); it != changes.constEnd(); it++)
		localState.insert(it.key(), it.value());
	localReady = true;
	reloadChangeList();
}

void ChangeController::updateLocalStatus(const StateHolder::ChangeKey &key, StateHolder::ChangeState &state)
{
	localState.insert(key, state);
	reloadChangeList();
}

void ChangeController::updateRemoteStatus(bool canUpdate, const StateHolder::ChangeHash &changes)
{
	for(auto it = changes.constBegin(); it != changes.constEnd(); it++)
		remoteState.insert(it.key(), it.value());
	remoteReady = canUpdate;
	reloadChangeList();
}

void ChangeController::reloadChangeList()
{
	if(remoteReady && !localReady)
		emit loadLocalStatus();
	else if(remoteReady && localReady) {

	} else {

	}
}
