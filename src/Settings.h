#pragma once

class Settings
{
public:
	using ISetting = AutoTOML::ISetting;
	using bSetting = AutoTOML::bSetting;

	static void load()
	{
		try {
			const auto table = toml::parse_file("Data/F4SE/Plugins/Buffout4.toml"s);
			for (const auto& setting : ISetting::get_settings()) {
				setting->load(table);
			}
		} catch (const toml::parse_error& e) {
			std::ostringstream ss;
			ss
				<< "Error parsing file \'" << *e.source().path << "\':\n"
				<< '\t' << e.description() << '\n'
				<< "\t\t(" << e.source().begin << ')';
			logger::error("{}", ss.str());
			stl::report_and_fail("failed to load settings"sv);
		} catch (const std::exception& e) {
			stl::report_and_fail(e.what());
		} catch (...) {
			stl::report_and_fail("unknown failure"sv);
		}
	}

	static inline bSetting UnalignedLoad{ "Fixes"s, "UnalignedLoad"s, true };

	static inline bSetting MemoryManager{ "Patches"s, "MemoryManager"s, true };
	static inline bSetting MemoryManagerDebug{ "Patches"s, "MemoryManagerDebug"s, false };

private:
	Settings() = delete;
	Settings(const Settings&) = delete;
	Settings(Settings&&) = delete;

	~Settings() = delete;

	Settings& operator=(const Settings&) = delete;
	Settings& operator=(Settings&&) = delete;
};
