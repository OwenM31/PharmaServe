FROM gcc:13 AS build

RUN apt-get update && apt-get install -y \
	cmake \
	make \
	git \
	&& rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN mkdir -p build && cd build \
	&& cmake .. \
	&& cmake --build . --parallel

# Run tests 
RUN cd build && ./tests/pharmaserve_tests

ENTRYPOINT ["/app/build/pharmaserve"]
