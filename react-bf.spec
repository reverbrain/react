%if %{defined rhel} && 0%{?rhel} < 6
%define __python /usr/bin/python2.6
%endif
%{!?python_sitelib: %global python_sitelib %(%{__python} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())")}
%{!?python_sitearch: %global python_sitearch %(%{__python} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib(1))")}

Summary:	Distributed hash table storage
Name:		react
Version:	2.3.0
Release:	1%{?dist}

License:	GPLv2+
Group:		System Environment/Libraries
URL:		https://github.com/reverbrain/react
Source0:	%{name}-%{version}.tar.bz2
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

%if %{defined rhel} && 0%{?rhel} < 6
BuildRequires:	gcc44 gcc44-c++
%else
%endif
BuildRequires:	cmake 

%define boost_ver %{nil}

BuildRequires:	boost%{boost_ver}-devel

%description
React is library for embedding realtime monitoring into C++ applications

%package devel
Summary: Development files for %{name}
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}


%description devel
React is library for embedding realtime monitoring into C++ applications

This package contains libraries, header files and developer documentation.

%prep
%setup -q

%build
export LDFLAGS="-Wl,-z,defs"
export DESTDIR="%{buildroot}"
%if %{defined rhel} && 0%{?rhel} < 6
export PYTHON=/usr/bin/python26
export CC=gcc44
export CXX=g++44
CXXFLAGS="-pthread" %{cmake} .
%else
%{cmake} .
%endif

make %{?_smp_mflags}
make check

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}
rm -f %{buildroot}%{_libdir}/*.a
rm -f %{buildroot}%{_libdir}/*.la

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%{_libdir}/libreact.so.*

%files devel
%defattr(-,root,root,-)
%{_includedir}/*
%{_libdir}/libreact.so

%changelog
* Tue Apr 29 2014 Andrey Kashin <kashin.andrej@gmail.com> - 2.3.0
- web: amcharts js library sources added
- web: stacked_histograms added
- examples: extra newline removed
- refactoring: code simplification
- refactoring: aggregators removed, they should be moved outside
- api: flow of aggregators replaced with subthread_aggregator
- web: html templates added
- api: id stat added to call_tree
- web: histograms and call trees visualisation added
- web: config.py that stores Flask-WTF settings added
- web: web interface initial commit
- examples: batch_histogram_aggregator example added
- aggregators: batch_histogram_aggregator added
- aggregators: category_extractor can now convert to json
- examples: react_progress_submit example added
- api: Function to create subthread aggregator added. Docs fixed.

* Mon Apr 28 2014 Andrey Kashin <kashin.andrej@gmail.com> - 2.1.1
- examples: No lambdas :(

* Mon Apr 28 2014 Andrey Kashin <kashin.andrej@gmail.com> - 2.1.0
- tests: missing include added
- examples: subthread monitoring example added
- tests: refactoring
- tests: tests for react_add_stat(...) added
- api: NULL pointer object usage fixed
- api: function to create subthread aggregator added
- aggregators: call_tree replaced with concurrent_call_tree to avoid races
- aggregators: iterator dereference fixed
- api: missed thread_local specifier added
- benchmarks: code simplifying
- api, tests, benchmarks: Don't forget to set aggregator!
- build: more std::to_string fixes for gcc-4.4
- aggregators: aggregators names in json are consistent now
- build: missing std::to_string(int) fixed for gcc-4.4
- gcc: range-based for removed to compile on gcc-4.4
- build: boost-test-dev added to dependencies
- Merge remote-tracking branch 'origin/develop'
- build: libboost-dev added to dependencies

* Sat Apr 26 2014 Andrey Kashin <kashin.andrej@gmail.com> - 2.0.0
- api, examples, tests, benchmarks: react_context removed from public api
- aggregators: memory leak with strings pushed into rapidjson fixed
- examples: duplicate define_new_action removed
- api: actions_set is now static singleton
- aggregators: filter aggregator removed
- examples: category_filter_aggregator example added
- aggregators: category_filter_aggregator added
- aggregators: complex_aggregator added
- core: function to get call_tree's map<action_code, nodes> added
- aggregators: parent_call_tree aggregator added
- examples: histogram aggregator example added
- examples: stats example fixed
- tests: histogram tests added. fix in aggregators constructors
- aggregators: histogram aggregator added
- aggregators: actions_set is now passed to aggregator in constructor
- core: function to get all tree nodes with specified action_code added
- api: add_stat functions are now generated using define
- tests: basic stats tests added
- examples: filter_aggregator example added
- aggregators: filter aggregator added. You can filter aggregated trees with explicitly set filter function.
- examples: example of using stats added
- api, stats: public api for adding stats to tree added
- core: stat_values added to call_tree
- api, aggregators: Aggregator is passed as void* to react_activate now
- api, aggregators: react_activate now takes aggregator as parameter
- core: merge_into for merging call_tree into call_tree added
- tests, aggregators: unordered_call_tree_aggregator test added
- aggregator: unordered_call_tree refactoring
- copyrights: Copyrights added to aggregators
- aggregators: Aggregators are moved into separate folder, paths changed
- docs: code_is_valid method doxygen description added
- tests: simple aggregator test added
- aggregator: basic aggregation tools added
- utils: Function to print rapidjson value moved to utils.hpp
- tests: actions_set suite name fixed
- benchmark: Simple benchmarks for deep and flat tree added
- tests: ctest check during debian and rhel build added
- cmake, tests: enable_testing() and add_test() added to cmake
- tests: public_api test added
- cmake: tests are added to project. build options added.
- tests: call_tree_updater test added
- tests: call_tree test added
- tests: actions_set test added
- tests: Boost.Tests base added. CMakeLists.txt for tests added.

* Thu Apr 03 2014 Andrey Kashin <kashin.andrej@gmail.com> - 1.0.3
- build: fixed lucid build (gcc 4.4)
- Renaming time_stats to call_tree
- ordered_node_t now has start_time and stop_time

* Thu Mar 27 2014 Andrey Kashin <kashin.andrej@gmail.com> - 1.0.1
- Initial release.
