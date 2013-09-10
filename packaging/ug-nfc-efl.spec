%define _usrdir	/usr
%define _ugdir	%{_usrdir}/ug


Name:       ug-nfc-efl
Summary:    UI gadget about the nfc
Version:    0.0.8
Release:    0
Group:      Applications/Core Applications
License:    Flora
Source0:    %{name}-%{version}.tar.gz
Source1:    libug-setting-nfc-efl.install.in
Source2:    libug-share-nfc-efl.install.in
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(ui-gadget-1)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(utilX)
BuildRequires:  pkgconfig(notification)
BuildRequires:  pkgconfig(capi-appfw-application)
BuildRequires:  pkgconfig(capi-network-nfc)
BuildRequires:  pkgconfig(capi-content-mime-type)
BuildRequires:  pkgconfig(sqlite3)

BuildRequires:  cmake
BuildRequires:  edje-tools
BuildRequires:  gettext-tools

Requires(post): /sbin/ldconfig
Requires(post): /usr/bin/vconftool
Requires(postun): /sbin/ldconfig

%description
UI gadget about the nfc

%prep
%setup -q

%package devel
Summary:  UG for nfc setting
Group:    Development/Libraries
Requires: %{name} = %{version}-%{release}

%description devel
UG for nfc setting

%package -n ug-share-nfc-efl
Summary:  UG for nfc share
Group:    Development/Libraries
Requires(post): /sbin/ldconfig
Requires(post): /usr/bin/vconftool
Requires: %{name} = %{version}-%{release}

%description -n ug-share-nfc-efl
UG for nfc share


%build
mkdir cmake_tmp
cd cmake_tmp
cmake .. -DCMAKE_INSTALL_PREFIX=%{_ugdir}

make %{?jobs:-j%jobs}


%install
cd cmake_tmp
rm -rf %{buildroot}
%make_install
cd ..
mkdir -p %{buildroot}/usr/share/license
mkdir -p %{buildroot}/etc/config/nfc/

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%post -n ug-share-nfc-efl
/sbin/ldconfig

%postun -n ug-share-nfc-efl
/sbin/ldconfig

%files
%manifest ug-nfc-efl.manifest
%defattr(-,root,root,-)
%license LICENSE.Flora
/usr/ug/lib/libug-setting-nfc-efl*
/usr/ug/res/locale/*/LC_MESSAGES/ug-setting-nfc-efl*
/usr/ug/res/icons/*
/etc/smack/accesses2.d/ug.setting-nfc-efl.include

%files -n ug-share-nfc-efl
%manifest ug-nfc-efl.manifest
%defattr(-,root,root,-)
%license LICENSE.Flora
/usr/ug/lib/libug-share-nfc-efl*
/usr/ug/res/locale/*/LC_MESSAGES/ug-share-nfc-efl*
/usr/ug/res/images/*
/usr/ug/res/edje/*
/etc/smack/accesses2.d/ug.share-nfc-efl.include
/etc/config/nfc/*
