%if "%{?profile}" == "mobile"
%global PREFIX  /usr/ug
%endif

%if "%{?profile}" == "wearable"
%global PREFIX  %{_prefix}/apps/org.tizen.nfc-setting-app/
%endif

%if "%{?profile}" == "tv"
ExcludeArch: %{arm} %ix86 x86_64
%endif

%if "%{?profile}" == "common"
%global PREFIX /usr/ug
%endif

Name:       ug-nfc-efl
Summary:    NFC Setting UI
Version:    3.1.0
Release:    0
Group:      App/Network
License:    Flora-1.1
Source0:    %{name}-%{version}.tar.gz

%if "%{?profile}" == "mobile"
BuildRequires:  pkgconfig(ui-gadget-1)
BuildRequires:  pkgconfig(notification)
BuildRequires:  pkgconfig(capi-content-mime-type)
BuildRequires:  pkgconfig(capi-appfw-app-manager)
BuildRequires:  pkgconfig(sqlite3)
BuildRequires:  pkgconfig(notification)
%endif

%if "%{?profile}" == "wearable"
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(pkgmgr-info)
BuildRequires:  pkgconfig(syspopup)
%endif

%if "%{?profile}" == "common"
BuildRequires:  pkgconfig(ui-gadget-1)
BuildRequires:  pkgconfig(notification)
BuildRequires:  pkgconfig(capi-content-mime-type)
BuildRequires:  pkgconfig(capi-appfw-app-manager)
BuildRequires:  pkgconfig(sqlite3)
BuildRequires:  pkgconfig(notification)
%endif

BuildRequires:  pkgconfig(capi-network-nfc)
BuildRequires:  pkgconfig(capi-appfw-application)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(efl-extension)
BuildRequires:  pkgconfig(elementary)
BuildRequires:  cmake
BuildRequires:  edje-tools
BuildRequires:  gettext-tools
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
NFC Setting UI

%prep
%setup -q

%build
cmake . -DCMAKE_INSTALL_PREFIX=%{PREFIX} \
%if "%{?profile}" == "mobile"
	-DTIZEN_MOBILE=1
%endif
%if "%{?profile}" == "wearable"
	-DTIZEN_WEARABLE=1
%endif
%if "%{?profile}" == "common"
	-DTIZEN_COMMON=1
%endif

make %{?jobs:-j%jobs}

%install
%make_install

mkdir -p %{buildroot}/usr/share/license
cp -af %{_builddir}/%{name}-%{version}/LICENSE.Flora %{buildroot}/usr/share/license/

%post
/sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_datadir}/license/LICENSE.Flora
%{_datadir}/packages/*
%{PREFIX}/*
