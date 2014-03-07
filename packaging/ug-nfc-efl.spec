%define _usrdir	/usr
%define _ugdir	%{_usrdir}/ug


Name:       ug-nfc-efl
Summary:    UI gadget about the nfc
Version:    0.0.9
Release:    0
Group:      Applications/Network
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
BuildRequires:  pkgconfig(x11)

BuildRequires:  pkgconfig(libtzplatform-config)
BuildRequires:  cmake
BuildRequires:  edje-tools
BuildRequires:  gettext-tools


%description
UI gadget about the nfc

%prep
%setup -q

%package devel
Summary:  ug for nfc setting
Requires: %{name} = %{version}-%{release}

%description devel
ug for nfc setting

%package -n ug-share-nfc-efl
Summary:  ug for nfc share
Requires: %{name} = %{version}-%{release}

%description -n ug-share-nfc-efl
ug for nfc share


%build
mkdir cmake_tmp
cd cmake_tmp
%cmake .. -DCMAKE_INSTALL_PREFIX=%{_ugdir}

make %{?jobs:-j%jobs}


%install
cd cmake_tmp
%make_install
cd ..
mkdir -p %{buildroot}/etc/config/nfc/

%find_lang ug-setting-nfc-efl
%find_lang ug-share-nfc-efl

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%post -p /sbin/ldconfig -n ug-share-nfc-efl

%postun -p /sbin/ldconfig -n ug-share-nfc-efl

%files -f ug-setting-nfc-efl.lang
%manifest ug-nfc-efl.manifest
%defattr(-,root,root,-)
/usr/ug/lib/libug-setting-nfc-efl*
/usr/ug/res/icons/*
/etc/smack/accesses2.d/ug.setting-nfc-efl.include
%license LICENSE.Flora

%files -n ug-share-nfc-efl -f ug-share-nfc-efl.lang
%defattr(-,root,root,-)
%manifest ug-nfc-efl.manifest
%license LICENSE.Flora
/usr/ug/lib/libug-share-nfc-efl*
/usr/ug/res/images/*
/usr/ug/res/edje/*
/etc/smack/accesses2.d/ug.share-nfc-efl.include
/etc/config/nfc/*
