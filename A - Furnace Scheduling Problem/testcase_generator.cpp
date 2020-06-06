// testcase_generator.cpp Ver.5.0

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <numeric>
#include <chrono>

constexpr auto delim = '\t';
constexpr auto header_s = "HEADER";
constexpr auto machine_s = "MACHINE";
constexpr auto bom_s = "BOM";
constexpr auto combi_s = "COMBI";
constexpr auto order_s = "ORDER";

constexpr auto bom_appear_pro = 0.5;

constexpr auto day_sec = 86400U;
constexpr auto time_limit = day_sec * 1000U;
constexpr auto holiday_limit = time_limit / 4U;
constexpr auto deadline_limit = time_limit / 10U;

#ifdef __GNUC__
struct GCCrandseed {
	unsigned long long operator()() const {
		return static_cast<unsigned long long>(std::chrono::system_clock::now().time_since_epoch().count());
	}
};
#endif 

using random_seed =
#ifdef __GNUC__
	GCCrandseed;
#else
	std::random_device;
#endif
using random_engine = std::default_random_engine;

template<typename T> using random_time = std::exponential_distribution<T>;
template<typename T> using random_count = std::uniform_int_distribution<T>;
template<typename T> using random_cost = std::normal_distribution<T>;
using random_bin = std::bernoulli_distribution;
template<typename T> using random_select = std::binomial_distribution<T>;

template<typename T> constexpr std::common_type_t<T, decltype(day_sec)> day_to_sec(T day) noexcept {
	return static_cast<std::common_type_t<T, decltype(day_sec)>>(day_sec) * static_cast<std::common_type_t<T, decltype(day_sec)>>(day);
}

template<typename T> constexpr std::common_type_t<T, decltype(day_sec)> sec_to_day(T sec) noexcept {
	using type = std::common_type_t<T, decltype(day_sec)>;
	return (static_cast<type>(sec) + static_cast<type>(day_sec) - static_cast<type>(1)) / static_cast<type>(day_sec);
}

template<typename T> constexpr T clamp(T value, T min, T max) {
	return std::min(std::max(min, value), max);
}

struct options {
	//オプション引数 --- option arguments
	bool h = false;
	unsigned M = 20, I = 20, R = 1000, C = 100000, ST = 10000, CC = 100000, Q = 100, E = 20000, Dmin = 10000, Dmax = 100000;
	unsigned A = 1000, PR = 10000, PE = 100, RQmin = 10, RQmax = 100, EC = 10000, DP = 0, QM = 2, HC = 7, IH = 5, NH = 0;
	options(const std::vector<std::string>& opts) {
		for (const auto& o : opts) {
			try {
				switch (o[1]) {
				case 'h':		// オプション引数解析 --- perse option arguments
					h = true;
					break;
				case 'A':
					A = std::stoul(o.substr(2));
					break;
				case 'C':
					if (o[2] == 'C') {
						CC = std::stoul(o.substr(3));
					}
					else {
						C = std::stoul(o.substr(2));
					}
					break;
				case 'D':
					if (o[2] == 'P') {
						DP = std::stoul(o.substr(3));
					}
					else if (o.substr(2, 3) == "min") {
						Dmin = std::stoul(o.substr(5));
					}
					else if (o.substr(2, 3) == "max") {
						Dmax = std::stoul(o.substr(5));
					}
					else {
						throw std::logic_error("error");
					}
					break;
				case 'E':
					if (o[2] == 'C') {
						EC = std::stoul(o.substr(3));
					}
					else {
						E = std::stoul(o.substr(2));
					}
					break;
				case 'H':
					if (o[2] == 'C') {
						HC = std::stoul(o.substr(3));
					}
					else {
						throw std::logic_error("error");
					}
					break;
				case 'I':
					if (o[2] == 'H') {
						IH = std::stoul(o.substr(3));
					}
					else {
						I = std::stoul(o.substr(2));
					}
					break;
				case 'M':
					M = std::stoul(o.substr(2));
					break;
				case 'N':
					if (o[2] == 'H') {
						NH = std::stoul(o.substr(3));
					}
					else {
						throw std::logic_error("error");
					}
					break;
				case 'P':
					if (o[2] == 'R') {
						PR = std::stoul(o.substr(3));
					}
					else if (o[2] == 'E') {
						PE = std::stoul(o.substr(3));
					}
					else {
						throw std::logic_error("error");
					}
					break;
				case 'Q':
					if (o[2] == 'M') {
						QM = std::stoul(o.substr(3));
					}
					else {
						Q = std::stoul(o.substr(2));
					}
					break;
				case 'R':
					if (o.substr(2, 4) == "Qmin") {
						RQmin = std::stoul(o.substr(6));
					}
					else if (o.substr(2, 4) == "Qmax") {
						RQmax = std::stoul(o.substr(6));
					}
					else {
						R = std::stoul(o.substr(2));
					}
					break;
				case 'S':
					if (o[2] == 'T') {
						ST = std::stoul(o.substr(3));
					}
					else {
						throw std::logic_error("error");
					}
					break;
				default:
					std::cerr << "invalid option : " << o << std::endl;
					exit(1);
					break;
				}
			}
			catch (...) {
				std::cerr << "option error : " << o << std::endl;
				exit(1);
			}
		}
	}
};

class testcase {
private:
	struct header {
		unsigned M = 0U, I = 0U, R = 0U, BL = 0U, CL = 0U;
		void print(std::ostream& out) const {
			out << header_s << delim << M << delim << I << delim << R << delim << BL << delim << CL << std::endl;
		}
	} header;
	struct machine {
		unsigned m = 0U;
		unsigned rq = 1U;
		std::vector<unsigned> md;
		void print(std::ostream& out) const {
			out << machine_s << delim << m  << delim << rq << delim << md.size();
			for (const auto holiday : md) { out << delim << holiday; }
			out << std::endl;
		}
	};
	std::vector<machine> machine;
	struct bom {
		unsigned i = 0U, m = 0U, t = 0U, ec = 0U;
		void print(std::ostream& out) const {
			out << bom_s << delim << i << delim << m << delim << t << delim << ec << std::endl;
		}
	};
	std::vector<bom> bom;
	struct combi {
		unsigned m = 0U, ipre = 0U, inext = 0U, ct = 0U, cc = 0U;
		void print(std::ostream& out) const {
			out << combi_s << delim << m << delim << ipre << delim << inext << delim << ct << delim << cc << std::endl;
		}
	};
	std::vector<combi> combi;
	struct order {
		unsigned r = 0U, i = 0U, e = 0U, d = 0U, a = 0U, q = 0U, qmin = 0U, pr = 0U;
		void print(std::ostream& out) const {
			out << order_s << delim << r << delim << i << delim << e << delim << d << delim << q << delim << qmin << delim << pr << delim << a << std::endl;
		}
	};
	std::vector<order> order;
	
	auto generate_bom(const options& opt) {
		const auto I = opt.I, M = opt.M;
		const auto bake_time_max = opt.C;
		const auto bake_time_min = std::max(1U, bake_time_max / 100U);
		const auto bake_time_ave = std::max(bake_time_min + 1U, bake_time_max / 10U);
		const auto electric_cost_max = opt.EC;
		const auto electric_cost_min = std::max(1U, electric_cost_max / 100U);
		const auto electric_cost_ave = std::max(1U, electric_cost_max / 2U);
		const auto electric_cost_var = std::max(1U, electric_cost_max / 4U);
		std::vector<std::vector<unsigned>> bom_table(M);		// bom_table[m] : パン焼き機mに対して作成可能なパンの種類iの集合 --- the set of items which can product by machine m.
		std::vector<unsigned> max_time(M);						// max_time[m] : パン焼き機mに対する最大焼き時間 --- longest time to bake in machine m.
		random_seed sed;
		random_engine eng(sed());
		random_select<unsigned> rand_select(M, bom_appear_pro);
		random_time<double> rand_time(1. / static_cast<double>(bake_time_ave - bake_time_min));
		random_cost<double> rand_cost(electric_cost_ave, electric_cost_var);
		for (auto i = 0U; i < I; i++) {
			std::vector<unsigned> vec(M);
			std::iota(std::begin(vec), std::end(vec), 0U);
			std::shuffle(std::begin(vec), std::end(vec), eng);
			auto r = clamp(rand_select(eng), 1U, M);
			for (auto j = 0U; j < r; j++) {
				bom_table[vec[j]].push_back(i);
				auto time = clamp(static_cast<unsigned>(rand_time(eng)) + bake_time_min, bake_time_min, bake_time_max);
				max_time[vec[j]] = std::max(max_time[vec[j]], time);
				bom.push_back({i, vec[j], time, clamp(static_cast<unsigned>(std::max(0., rand_cost(eng))), electric_cost_min, electric_cost_max)});
			}
		}
		// 未使用のパン焼き機を探す --- find unused machines.
		std::vector<bool> used_machine(M, false);
		for (const auto& b : bom) {
			used_machine[b.m] = true;
		}
		for (auto m = 0U; m < M; m++) {
			if (!used_machine[m]) {
				auto i = random_count<unsigned>(0U, I - 1U)(eng);
				bom_table[m].push_back(i);
				auto time = clamp(static_cast<unsigned>(rand_time(eng)) + bake_time_min, bake_time_min, bake_time_max);
				max_time[m] = std::max(max_time[m], time);
				bom.push_back({i, m, time, clamp(static_cast<unsigned>(std::max(0., rand_cost(eng))), electric_cost_min, electric_cost_max)});
			}
		}
		// 整列 --- sort
		std::sort(std::begin(bom), std::end(bom), [](const decltype(bom)::value_type& l, const decltype(bom)::value_type& r) {return l.i < r.i || (l.i == r.i && l.m < r.m); });
		for (auto& bt : bom_table) {
			std::sort(std::begin(bt), std::end(bt));
		}
		return std::make_pair(std::move(bom_table), std::move(max_time));
	}

	auto generate_combi(const options& opt, const std::vector<std::vector<unsigned>>& bom_table, std::vector<unsigned>&& max_t) {
		auto max_time = std::move(max_t);
		const auto M = opt.M;
		const auto setup_time_max = opt.ST;
		const auto setup_time_min = std::max(1U, setup_time_max / 100U);
		const auto setup_time_ave = std::max(setup_time_min + 1U, setup_time_max / 10U);
		const auto setup_cost_max = opt.CC;
		const auto setup_cost_min = std::max(1U, setup_cost_max / 100U);
		const auto setup_cost_ave = std::max(1U, setup_cost_max / 2U);
		const auto setup_cost_var = std::max(1U, setup_cost_max / 4U);
		random_seed sed;
		random_engine eng(sed());
		random_time<double> rand_time(1. / static_cast<double>(setup_time_ave - setup_time_min));
		random_cost<double> rand_cost(setup_cost_ave, setup_cost_var);
		// COMBI全通り探索 --- all COMBInations
		for (auto m = 0U; m < M; m++) {
			for(auto ipre : bom_table[m]) {
				for(auto inext : bom_table[m]) {
					if (ipre == inext || opt.ST == 0) {	// 同一品目では段取り時間、コストはゼロ --- setup time and cost between same items are zero.
						combi.push_back({m, ipre, inext, 0U, 0U});
					}
					else {
						auto time = clamp(static_cast<unsigned>(rand_time(eng)) + setup_time_min, setup_time_min, setup_time_max);
						combi.push_back({m, ipre, inext, time, clamp(static_cast<unsigned>(std::max(0., rand_cost(eng))), setup_cost_min, setup_cost_max)});
						max_time[m] = std::max(time, max_time[m]);
					}
				}
			}
		}
		return max_time;
	}

	auto generate_machine(const options& opt, const std::vector<std::vector<unsigned>>& bom_table, const std::vector<unsigned>& max_time) {
		const auto M = opt.M, I = opt.I;
		const auto operating_day_max = sec_to_day(std::max(opt.C, opt.ST)) * 5U;
		const auto operating_day_min = std::max(1U, operating_day_max / 5U);
		const auto operating_day_ave = std::max(operating_day_min + 1U, operating_day_max / 2U);
		const auto holiday_max = std::max(1U, operating_day_max / 2U);
		const auto holiday_min = std::max(1U, holiday_max / 5U);
		const auto holiday_ave = std::max(holiday_min + 1U, holiday_max / 2U);
		const auto parallel_min = std::max(1U, opt.RQmin), parallel_max = opt.RQmax;
		machine.resize(M);
		random_seed sed;
		random_engine eng(sed());
		random_count<unsigned> rand_parallel_count(parallel_min, parallel_max);
		random_bin rand_holiday(2. / 7.);
		random_time<double> rand_long_holidays_length(2. / std::max(opt.IH, 1U));
		random_count<unsigned> rand_holiday_length(1U, std::max(opt.HC / 3U, 1U));
		random_count<unsigned> rand_long_holidays_start(0U, 29U);
		std::vector<unsigned> min_rq(I, std::numeric_limits<unsigned>::max());
		std::vector<bool> hol_flag(M, true);
		std::fill_n(std::begin(hol_flag), std::min(M, opt.NH), false);
		std::shuffle(std::begin(hol_flag), std::end(hol_flag), eng);
		for (auto m = 0U; m < M; m++) {
			machine[m].m = m;
			machine[m].rq = rand_parallel_count(eng);
			constexpr auto limit = sec_to_day(holiday_limit);
			if (hol_flag[m]) {		// 休日のあるパン焼き機かどうか --- whether the machine has holiday or not.
				auto hol = rand_holiday_length(eng);
				auto ope = std::max(opt.HC, 1U) - hol;
				std::vector<bool> vec;
				auto ope_min = sec_to_day(max_time[m]);
				vec.reserve(static_cast<unsigned>(limit * 1.2));
				while (vec.size() < limit + 30U) {
					std::fill_n(std::back_inserter(vec), ope, true);
					std::fill_n(std::back_inserter(vec), hol, false);
				}
				std::rotate(std::begin(vec), std::begin(vec) + random_count<unsigned>(0U, static_cast<unsigned>(vec.size()) - 1U)(eng), std::end(vec));
				if (opt.IH != 0) {
					for (auto i = 0; i < (limit + 29) / 30; i++) {		// 不定休日 --- irregular holidays
						auto it = std::begin(vec) + i * 30LL;
						auto n = std::min(std::max(1U, std::min(static_cast<unsigned>(rand_long_holidays_length(eng)), opt.IH)), static_cast<unsigned>(std::end(vec) - it));
						std::fill_n(it + rand_long_holidays_start(eng), n, false);
					}
				}
				vec.resize(limit);
				auto count = 0U;
				for (const auto& b : vec) {
					if (!b) {
						machine[m].md.push_back(count);
					}
					count++;
				}
			}
			for (const auto& i : bom_table[m]) {
				min_rq[i] = std::min(min_rq[i], machine[m].rq);
			}
		}
		return min_rq;
	}

	void generate_order(const options& opt, const std::vector<unsigned>& min_rq) {
		const auto I = opt.I, R = opt.R, e_max = opt.E;
		const auto e_d_max = opt.Dmax, e_d_min = std::min(deadline_limit, opt.Dmin);
		const auto e_d_ave = std::max((e_d_max - e_d_min) / 10U, 1U) + e_d_min;
		const auto pr_max = opt.PR;
		const auto pr_min = std::max(1U, pr_max / 10U);
		const auto pr_ave = std::max(1U, pr_max / 2U);
		const auto pr_var = std::max(1U, pr_ave / 2U);
		const auto a_max = opt.A;
		const auto a_min = std::max(1U, a_max / 100U);
		const auto a_ave = std::max(a_min + 1U, a_max / 10U);
		const auto quantity_max = opt.Q, quantity_min = 1U;
		random_seed sed;
		random_engine eng(sed());
		random_count<unsigned> rand_item(0U, I - 1U);
		random_time<double> rand_e_d(1. / static_cast<double>(e_d_ave - e_d_min));
		random_count<unsigned> rand_e(0U, e_max);
		random_cost<double> rand_e1(e_max / 6U, e_max / 6U);
		random_cost<double> rand_e2(e_max / 2U, e_max / 6U);
		random_cost<double> rand_e3(e_max * 5U / 6U, e_max / 6U);
		random_time<double> rand_a(1. / static_cast<double>(a_ave - a_min));
		random_count<unsigned> rand_q(quantity_min, quantity_max);
		random_cost<double> rand_pr(pr_ave, pr_var);
		order.resize(R);
		std::vector<unsigned> items(R);
		for (auto i = 0U; i < R; i++) {
			items[i] = i < I ? i : rand_item(eng);		// パン全種類一個以上 --- all items must appear.
		}
		std::shuffle(std::begin(items), std::end(items), eng);
		std::vector<unsigned> select_e_rand(I);
		random_count<unsigned> select(0U, 2U);
		for (auto& e : select_e_rand) {
			e = select(eng);
		}
		auto index = 0U;
		for (auto& o : order) {
			o.r = index;
			o.i = items[index];
			if (opt.DP == 0) {		// デフォルト --- default
				o.e = rand_e(eng);
			}
			else {					// 最早開始時刻が品目ごとに偏る --- the earliest start times are biased by item.
				switch (select_e_rand[o.i]) {
				case 0:
					o.e = clamp(static_cast<unsigned>(std::max(0., rand_e1(eng))), 0U, e_max);
					break;
				case 1:
					o.e = clamp(static_cast<unsigned>(std::max(0., rand_e2(eng))), 0U, e_max);
					break;
				case 2:
					o.e = clamp(static_cast<unsigned>(std::max(0., rand_e3(eng))), 0U, e_max);
					break;
				}
			}
			o.d = std::min(deadline_limit, o.e + clamp(static_cast<unsigned>(rand_e_d(eng) + e_d_min), e_d_min, e_d_max));
			o.a = clamp(static_cast<unsigned>(rand_a(eng) + a_min), a_min, a_max);
			if (opt.QM == 0) {			// qmin == q 分割禁止 --- prohibit splitting
				o.q = o.qmin =  random_count<unsigned>(quantity_min, min_rq[o.i])(eng);
			}
			else if (opt.QM == 1) {		// qmin == 1 自由に分割可能 --- split freely
				o.q = rand_q(eng);
				o.qmin = 1U;
			}
			else {						// qmin : ランダム --- random 
				o.q = rand_q(eng);
				auto mod = o.q % min_rq[o.i];
				auto sup = o.q / 2U + 1U;
				o.qmin = random_count<unsigned>(1U, std::min(sup <= min_rq[o.i] ? sup : sup - 1U, mod == 0U ? min_rq[o.i] : mod))(eng);
			}
			o.pr = clamp(static_cast<unsigned>(std::max(0., rand_pr(eng))), pr_min, pr_max);
			index++;
		}
	}

	void set_header(const options& opt) {
		header.M = opt.M;
		header.I = opt.I;
		header.R = opt.R;
		header.BL = static_cast<unsigned>(bom.size());
		header.CL = static_cast<unsigned>(combi.size());
	}

public:
	
	void generate_testcase(const options& opt) {
		auto pair = generate_bom(opt);
		auto bt = std::move(pair.first);
		auto mt2 = generate_combi(opt, bt, std::move(pair.second));
		auto mr = generate_machine(opt, bt, mt2);
		generate_order(opt, mr);
		set_header(opt);
	}

	void print(std::ostream& out = std::cout) const {
		header.print(out);
		for (auto& m : machine) { m.print(out); }
		for (auto& b : bom) { b.print(out); }
		for (auto& c : combi) { c.print(out); }
		for (auto& o : order) { o.print(out); }
	}
	
};

std::ostream& operator<<(std::ostream& out, const testcase& tc) {
	tc.print(out);
	return out;
}

// helpメニュー --- help menu
void help_message(const std::string& exe) {
	std::cout << "usage :" << std::endl <<
		"\t>" << exe << " [-options] [filenames]" << std::endl <<
		"options :" << std::endl <<
		"\t-h : display help." << std::endl <<
		"\t-M : the number of machines." << std::endl <<
		"\t-I : the number of items." << std::endl <<
		"\t-R : the number of orders." << std::endl <<
		"\t-C : the maximum capability of machines." << std::endl <<
		"\t-ST : the maximum setup time." << std::endl <<
		"\t-Q : the maximum production quantity of orders." << std::endl <<
		"\t-E : the latest time able to start baking." << std::endl <<
		"\t-Dmin : the minimum time from earliest start time until deadline." << std::endl <<
		"\t-Dmax : the maximum time from earliest start time until deadline." << std::endl <<
		"\t-A : the maximum permissible range of delay." << std::endl <<
		"\t-PR : the maximum gross profit." << std::endl <<
		"\t-PE : the maximum delay penalty rate." << std::endl <<
		"\t-RQmin : the minimum resource quantity." << std::endl <<
		"\t-RQmax : the maximum resource quantity." << std::endl <<
		"\t-DP : the earliest start time distribution. 0:uniform random, 1:depends on item number." << std::endl <<
		"\t-QM : the minimum quantity's distribution. 0:qmin = q, 1:qmin = 1, 2:qmin = random." << std::endl <<
		"\t-HC : the maximum length of the holiday's cycle. if 0 is specified, holiday appears randomly." << std::endl <<
		"\t-IH : the maximum length of holidays which appear in 30 days." << std::endl <<
		"\t-NH : the number of machines which have no holiday." << std::endl;
}

// コマンドライン引数をオプションと出力ファイル名に分別 --- separate commandline arguments output filenames from options.
auto separate_args(const std::vector<std::string>& args) {
	std::vector<std::string> opts;
	std::vector<std::string> file;
	for (const auto& a : args) {
		if (a[0] == '-') {
			opts.push_back(a);
		}
		else {
			file.push_back(a);
		}
	}
	return std::make_pair(std::move(opts), std::move(file));
}

int main(int argc, const char* argv[]) {
	std::vector<std::string> args(static_cast<long long>(argc) - 1);
	std::for_each(std::begin(args), std::end(args), [&argv, index = 1](auto& a) mutable { a = argv[index++]; });
	auto pair = separate_args(args);
	auto opts = std::move(pair.first);
	auto file = std::move(pair.second);
	options opt(opts);
	if (opt.h) {
		help_message(std::string(argv[0]));
		return 0;
	}
	if (file.size() == 0) {
		testcase tc;
		tc.generate_testcase(opt);
		tc.print();
	}
	for (auto& f : file) {			// ファイルの数だけ生成 --- generate as many as the number of files.
		std::ofstream out(f, std::ios::out);
		testcase tc;
		tc.generate_testcase(opt);
		out << tc;
	}
	return 0;
}

