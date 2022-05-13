.PHONY: clean socket_example build_zmq_example

clean:
	rm -rf build_socket_example build_zmq_example
	rm -f ./example/example_zmq_helper
	rm -f ./example/example_socket_helper

socket_example:
	rm -rf build_socket_example
	mkdir build_socket_example
	cd build_socket_example; \
	cmake .. -DUSE_ZMQPP=OFF -DBUILD_EXAMPLE=ON ; \
	cmake --build . ; \
	cmake --install .

zmq_example:
	rm -rf build_zmq_example
	mkdir build_zmq_example
	cd build_zmq_example; \
	cmake .. -DUSE_ZMQPP=ON -DBUILD_EXAMPLE=ON ; \
	cmake --build . ; \
	cmake --install .