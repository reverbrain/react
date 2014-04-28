#ifndef TESTS_HPP
#define TESTS_HPP

#include <iostream>

#include <boost/test/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>

struct stream_redirect {
	stream_redirect(std::ostream &stream, std::streambuf *new_buffer):
		stream(stream), old(stream.rdbuf(new_buffer)) {}

	~stream_redirect() {
		stream.rdbuf(old);
	}

private:
	std::ostream &stream;
	std::streambuf *old;
};

struct cerr_redirect {
	cerr_redirect(std::streambuf *new_buffer):
		redirect(std::cerr, new_buffer) {}

private:
	stream_redirect redirect;
};

struct cout_redirect {
	cout_redirect(std::streambuf *new_buffer):
		redirect(std::cout, new_buffer) {}

private:
	stream_redirect redirect;
};

#endif // TESTS_HPP
