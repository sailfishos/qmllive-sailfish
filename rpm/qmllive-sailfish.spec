Name: qmllive-sailfish
Version: 0
Release: 1
Summary: Qt QmlLive extensions for Sailfish OS application development
License: GPLv3+ and BSD
URL: https://github.com/sailfishos/qmllive-sailfish
Source: %{name}-%{version}.tar.bz2
BuildRequires: pkgconfig(qmllive)
BuildRequires: pkgconfig(Qt5Gui)
BuildRequires: pkgconfig(Qt5Quick)
BuildRequires: pkgconfig(Qt5Qml)
BuildRequires: pkgconfig(sailfishapp) >= 1.1
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
Enables use of Qt QmlLive for Sailfish OS application development.

%prep
%setup -q

%build

%qmake5
make %{?_smp_mflags}

%install
make install INSTALL_ROOT=%{buildroot}
# Workaround for building inside SDK VM where the shared filesystem is noexec
chmod +x %{buildroot}/%{_bindir}/qmlliveruntime-sailfish

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_bindir}/qmlliveruntime-sailfish
%{_libdir}/qmllive-sailfish/libsailfishapp-preload.so
