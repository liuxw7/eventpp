// eventpp library
// Copyright (C) 2018 Wang Qi (wqking)
// Github: https://github.com/wqking/eventpp
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//   http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "test.h"
#include "eventpp/eventqueue.h"
#include "eventpp/utilities/anydata.h"

#include <thread>
#include <vector>

namespace {

struct Event {
	int type;
};

struct EventA : Event {
	int a;
};

struct EventB : Event {
	int b1;
	int b2;
};

void doExecuteEventQueue(
		const std::string & message,
		const size_t queueSize,
		const size_t iterateCount,
		const size_t eventCount,
		size_t listenerCount = 0
	)
{
	using SP = std::shared_ptr<Event>;
	using EQ = eventpp::EventQueue<size_t, void (const SP &)>;
	EQ eventQueue;
	
	if(listenerCount == 0) {
		listenerCount = eventCount;
	}

	for(size_t i = 0; i < listenerCount; ++i) {
		eventQueue.appendListener(i % eventCount, [](const SP &) {});
	}
	
	const uint64_t time = measureElapsedTime([
			queueSize,
			iterateCount,
			eventCount,
			listenerCount,
			&eventQueue
		]{
		for(size_t iterate = 0; iterate < iterateCount; ++iterate) {
			for(size_t i = 0; i < queueSize; ++i) {
				eventQueue.enqueue(i % eventCount, std::make_shared<EventA>());
				eventQueue.enqueue(i % eventCount, std::make_shared<EventB>());
			}
			eventQueue.process();
		}
	});
	
	std::cout
		<< message
		<< " queueSize: " << queueSize
		<< " iterateCount: " << iterateCount
		<< " eventCount: " << eventCount
		<< " listenerCount: " << listenerCount
		<< " Time: " << time
		<< std::endl;
	;
}

void doExecuteEventQueueWithAnyData(
		const std::string & message,
		const size_t queueSize,
		const size_t iterateCount,
		const size_t eventCount,
		size_t listenerCount = 0
	)
{
	constexpr std::size_t maxSize = sizeof(EventB) * 2;
	using Data = eventpp::AnyData<maxSize>;
	struct Policies {
		using Callback = std::function<void (const Event &)>;
	};
	using EQ = eventpp::EventQueue<size_t, void (const Data &), Policies>;
	EQ eventQueue;
	
	if(listenerCount == 0) {
		listenerCount = eventCount;
	}

	for(size_t i = 0; i < listenerCount; ++i) {
		eventQueue.appendListener(i % eventCount, [](const Event &) {});
	}
	
	const uint64_t time = measureElapsedTime([
			queueSize,
			iterateCount,
			eventCount,
			listenerCount,
			&eventQueue
		]{
		for(size_t iterate = 0; iterate < iterateCount; ++iterate) {
			for(size_t i = 0; i < queueSize; ++i) {
				eventQueue.enqueue(i % eventCount, EventA());
				eventQueue.enqueue(i % eventCount, EventB());
			}
			eventQueue.process();
		}
	});
	
	std::cout
		<< message
		<< " queueSize: " << queueSize
		<< " iterateCount: " << iterateCount
		<< " eventCount: " << eventCount
		<< " listenerCount: " << listenerCount
		<< " Time: " << time
		<< std::endl;
	;
}


} //unnamed namespace

TEST_CASE("b8, EventQueue, AnyData")
{
	doExecuteEventQueue("Without AnyData", 100, 1000 * 100, 100);
	doExecuteEventQueueWithAnyData("With AnyData", 100, 1000 * 100, 100);
}

