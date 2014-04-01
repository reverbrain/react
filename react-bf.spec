%if %{defined rhel} && 0%{?rhel} < 6
%define __python /usr/bin/python2.6
%endif
%{!?python_sitelib: %global python_sitelib %(%{__python} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())")}
%{!?python_sitearch: %global python_sitearch %(%{__python} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib(1))")}

Summary:	Distributed hash table storage
Name:		react
Version:	1.0.1
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
%{_bindir}/*
%{_libdir}/libreact.so.*

%files devel
%defattr(-,root,root,-)
%{_includedir}/*
%{_libdir}/libreact.so

%changelog
* Thu Mar 27 2014 Andrey Kashin <kashin.andrej@gmail.com> - 1.0.1
- Initial release.
