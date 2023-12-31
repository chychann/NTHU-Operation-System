#include <assert.h>
#include <stdlib.h>
#include "ts_queue.hpp"
#include "item.hpp"
#include "reader.hpp"
#include "writer.hpp"
#include "producer.hpp"
#include "consumer_controller.hpp"

#define READER_QUEUE_SIZE 200
#define WORKER_QUEUE_SIZE 200
#define WRITER_QUEUE_SIZE 4000
#define CONSUMER_CONTROLLER_LOW_THRESHOLD_PERCENTAGE 20
#define CONSUMER_CONTROLLER_HIGH_THRESHOLD_PERCENTAGE 80
#define CONSUMER_CONTROLLER_CHECK_PERIOD 1000000

int main(int argc, char** argv) {
	assert(argc == 4);

	int n = atoi(argv[1]);
	std::string input_file_name(argv[2]);
	std::string output_file_name(argv[3]);

	// TODO: implements main function
	
	// define all queue
	TSQueue<Item*>* Input_queue;
	TSQueue<Item*>* Worker_queue;
	TSQueue<Item*>* Writer_queue;

	Input_queue = new TSQueue<Item*>(READER_QUEUE_SIZE);
	Worker_queue = new TSQueue<Item*>(WORKER_QUEUE_SIZE);
	Writer_queue = new TSQueue<Item*>(WRITER_QUEUE_SIZE);

	// define Transformer
	Transformer* transformer = new Transformer;

	// define Reader and Writer
	Reader* reader = new Reader(n, input_file_name, Input_queue);
	Writer* writer = new Writer(n, output_file_name, Writer_queue);

	// define 4 producers
	Producer* p1 = new Producer(Input_queue, Worker_queue, transformer);
	Producer* p2 = new Producer(Input_queue, Worker_queue, transformer);
	Producer* p3 = new Producer(Input_queue, Worker_queue, transformer);
	Producer* p4 = new Producer(Input_queue, Worker_queue, transformer);

	// define controller
	ConsumerController* controller = new ConsumerController(Worker_queue, Writer_queue, transformer, \
										CONSUMER_CONTROLLER_CHECK_PERIOD, CONSUMER_CONTROLLER_LOW_THRESHOLD_PERCENTAGE, \
										CONSUMER_CONTROLLER_HIGH_THRESHOLD_PERCENTAGE);
	// start all thread
	reader->start();
	writer->start();

	p1->start();
	p2->start();
	p3->start();
	p4->start();

	controller->start();

	reader->join();
	writer->join();
	

	// clean memory
	delete p2;
	delete p1;
	delete writer;
	delete reader;
	delete Input_queue;
	delete Worker_queue;
	delete Writer_queue;
	delete controller;
	delete p3;
	delete p4;

	return 0;
}
