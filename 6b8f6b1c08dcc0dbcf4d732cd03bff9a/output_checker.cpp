//output_checker.cpp Ver.5.1

//#define CHECK_INPUT
//#define LOCAL_TEST

#include <iostream>
#include <fstream>
#include <memory>
#include <utility>
#include <numeric>
#include <stdexcept>
#include <exception>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <cmath>
#include <cstdio>

using item_type = uint16_t;
using machine_type = uint16_t;
using line_type = uint32_t;
using cost_type = uint64_t;
using time_type = uint64_t;

constexpr time_type day_sec = 86400ULL;
constexpr time_type time_limit = day_sec * 1000ULL;
constexpr cost_type base_profit = 10000000000;
constexpr double point_rate = 1e10;

auto day_to_sec(time_type day) {
	if (std::numeric_limits<time_type>::max() / day_sec < day) {
		throw std::overflow_error("Overflow");
	}
	return day * day_sec;
}

// 切り捨て --- round down
constexpr auto sec_to_day_down(time_type sec) noexcept {
	return sec / day_sec;
}
// 切り上げ --- round up
constexpr auto sec_to_day_up(time_type sec) noexcept {
	return sec / day_sec + static_cast<time_type>(sec % day_sec == 0 ? 0 : 1);
}

class Input {
private:

	static std::unique_ptr<Input> input;
	Input() = default;

public:
	class Header {
	public:
		machine_type M = 0;
		item_type I = 0;
		line_type R = 0, BL = 0, CL = 0;
		void read(std::istream& in) {
			std::string s;
			in >> s;
#ifdef CHECK_INPUT
			if (s != "HEADER") {
				throw std::logic_error("Input Data is wrong : HEADER");
			}
#endif // CHECK_INPUT
			in >> M >> I >> R >> BL >> CL;
		}
	} header;


	class Bom {
	public:
		struct BomSub {
			item_type i;
			machine_type m;
			time_type t;
			cost_type ec;
		};

		std::vector<BomSub> vec;

		void read(std::istream& in) {
			vec.resize(input->header.BL);
			for (auto& v : vec) {
				std::string s;
				in >> s;
#ifdef CHECK_INPUT
				if (s != "BOM") {
					throw std::logic_error("Input Data is wrong : BOM");
				}
#endif // CHECK_INPUT
				item_type i;
				machine_type m;
				time_type t;
				cost_type ec;
				in >> i >> m >> t >> ec;
				v = {i, m, t, ec};
			}
#ifdef CHECK_INPUT
			if (!std::is_sorted(std::begin(vec), std::end(vec), [](const auto& l, const auto& r) {
				return l.i < r.i || (l.i == r.i && l.m < r.m);
			})) {
				throw std::logic_error("Input Data is wrong : BOM");
			}
#endif // CHECK_INPUT
		}
	} bom;

	class Combi {
	public:
		std::vector<std::map<std::pair<item_type, item_type>, std::pair<time_type, cost_type>>> vec;
		void read(std::istream& in) {
			vec.resize(input->header.M);
			for (line_type i = 0u; i < input->header.CL; i++) {
				std::string s;
				in >> s;
#ifdef CHECK_INPUT
				if (s != "COMBI") {
					throw std::logic_error("Input Data is wrong : COMBI");
				}
#endif // CHECK_INPUT
				machine_type m;
				item_type ipre, inext;
				time_type ct;
				cost_type cc;
				in >> m >> ipre >> inext >> ct >> cc;
				vec[m].emplace(std::make_pair(ipre, inext), std::make_pair(ct, cc));
			}
		}
	} combi;

	class Machine {
	public:
		std::vector<std::vector<time_type>> vec;
		std::vector<cost_type> rq;
		void read(std::istream& in) {
			vec.resize(input->header.M);
			rq.resize(input->header.M, 0ULL);
			for (machine_type i = 0U; i < input->header.M; i++) {
				std::string s;
				in >> s;
#ifdef CHECK_INPUT
				if (s != "MACHINE") {
					throw std::logic_error("Input Data is wrong : MACHINE");
				}
#endif // CHECK_INPUT
				machine_type m;
				time_type mn;
				in >> m >> rq[i] >> mn;
#ifdef CHECK_INPUT
				if (m != i) {
					throw std::logic_error("Input Data is wrong : MACHINE");
				}
#endif // CHECK_INPUT
				vec[i].resize(mn);
				for (auto& elem : vec[i]) {
					in >> elem;
				}
#ifdef CHECK_INPUT
				if (!std::is_sorted(std::begin(vec), std::end(vec))) {
					throw std::logic_error("Input Data is wrong : MACHINE");
				}
#endif // CHECK_INPUT
			}
		}
	} machine;

	class Order {
	public:
		struct OrderSub {
			item_type i;
			time_type e, d, a;
			cost_type pr, q, qmin;
		};
		std::vector<OrderSub> vec;
		void read(std::istream& in) {
			vec.resize(input->header.R);
			for (line_type index = 0U; index < input->header.R; index++) {
				std::string s;
				in >> s;
#ifdef CHECK_INPUT
				if (s != "ORDER") {
					throw std::logic_error("Input Data is wrong : ORDER");
				}
#endif // CHECK_INPUT
				line_type r;
				item_type i;
				time_type e, d, a;
				cost_type pr, q, qmin;
				in >> r >> i >> e >> d >> q >> qmin >> pr >> a;
#ifdef CHECK_INPUT
				if (r != index) {
					throw std::logic_error("Input Data is wrong : ORDER");
				}
#endif // CHECK_INPUT
				vec[index] = {i, e, d, a, pr, q, qmin};
			}
		}
	} order;

	void read(std::istream& in) {
		header.read(in);
		machine.read(in);
		bom.read(in);
		combi.read(in);
		order.read(in);
	}

	static void create() {
		if (input == nullptr) {
			input.reset(new Input());
		}
	}

	static void destroy() {
		input = nullptr;
	}

	static const Input& instance() {
		if (input == nullptr) {
			create();
		}
		return *input;
	}

	static void read_input(std::istream& in = std::cin) {
		input->create();
		input->read(in);
	}

	Input(const Input&) = delete;
	Input(Input&&) = delete;
};

void check_range(uint64_t x, uint64_t min, uint64_t max, const char* message) {
	if (x < min || x > max) {
		throw std::logic_error(message);
	}
}

class Output {
private:

	static std::unique_ptr<Output> output;
	Output() = default;

public:
	class Order {
	public:
		struct OrderSub {
			line_type r;
			machine_type m;
			cost_type q;
			time_type t1, t2;
			line_type line_id;
		};
		std::vector<OrderSub> vec;

		void read_line(std::istream& in, line_type line_id) {
			std::string s;
			try {
				in >> s;
				auto r = std::stoull(s);
				check_range(r, 0ULL, static_cast<uint64_t>(Input::instance().header.R) - 1ULL, "Order id \"r\" is out of range.");
				auto r_ = static_cast<line_type>(r);

				in >> s;
				auto m = std::stoull(s);
				check_range(m, 0ULL, static_cast<uint64_t>(Input::instance().header.M) - 1ULL, "Machine id \"m\" is out of range.");
				auto m_ = static_cast<machine_type>(m);

				in >> s;
				auto q = std::stoull(s);
				check_range(q, static_cast<uint64_t>(Input::instance().order.vec[r_].qmin), static_cast<uint64_t>(Input::instance().machine.rq[m_]), "Quantity \"q\" is out of range.");
				auto q_ = static_cast<cost_type>(q);

				in >> s;
				auto t1 = std::stoull(s);
				check_range(t1, 0ULL, static_cast<uint64_t>(time_limit), "Start time \"t3\" is out of range.");
				auto t1_ = static_cast<time_type>(t1);

				in >> s;
				auto t2 = std::stoull(s);
				check_range(t2, 0ULL, static_cast<uint64_t>(time_limit), "End time \"t4\" is out of range.");
				auto t2_ = static_cast<time_type>(t2);

				vec.push_back(OrderSub{r_, m_, q_, t1_, t2_, line_id});
			}
			catch (std::exception& e) {
				std::cerr << e.what() << std::endl;
				throw std::logic_error("Output Data is wrong : ORDER\nline : " + std::to_string(line_id));
			}
		}
	} order;

	class Combi {
	public:
		struct CombiSub {
			machine_type m;
			time_type t1, t2;
			line_type line_id;
		};

		std::vector<CombiSub> vec;

		void read_line(std::istream& in, line_type line_id) {
			std::string s;
			try {
				in >> s;
				auto m = std::stoull(s);
				check_range(m, 0ULL, static_cast<uint64_t>(Input::instance().header.M) - 1ULL, "Machine id \"m\" is out of range.");
				auto m_ = static_cast<machine_type>(m);

				in >> s;
				auto t1 = std::stoull(s);
				check_range(t1, 0ULL, time_limit, "Start time \"t1\" is out of range.");
				auto t1_ = static_cast<time_type>(t1);

				in >> s;
				auto t2 = std::stoull(s);
				check_range(t2, 0ULL, time_limit, "End time \"t2\" is out of range.");
				auto t2_ = static_cast<time_type>(t2);

				vec.push_back(CombiSub{m_, t1_, t2_, line_id});
			}
			catch (std::exception& e) {
				std::cerr << e.what() << std::endl;
				throw std::logic_error("Output Data is wrong : COMBI");
			}
		}

	} combi;

	void read(std::istream& in) {
		line_type line_id = 1;
		while (!in.eof()) {
			std::string s;
			in >> s;
			if (s == "ORDER") {
				order.read_line(in, line_id);
			}
			else if (s == "COMBI") {
				combi.read_line(in, line_id);
			}
			else if (in.eof()) {
				break;
			}
			else {
				s += " is not proper chunk. Output Data is wrong.\n";
				s += "line : " + std::to_string(line_id);
				throw std::logic_error(s);
			}
			line_id++;
		}
		std::sort(std::begin(output->order.vec), std::end(output->order.vec), [](const Order::OrderSub& l, const Order::OrderSub& r) {
			return l.m < r.m || (l.m == r.m && l.t1 < r.t1);
		});
		std::sort(std::begin(output->combi.vec), std::end(output->combi.vec), [](const Combi::CombiSub& l, const Combi::CombiSub& r) {
			return l.m < r.m || (l.m == r.m && l.t1 < r.t1);
		});
	}

	static void create() {
		if (output == nullptr) {
			output.reset(new Output);
		}
	}

	static void destroy() {
		output = nullptr;
	}

	static const Output& instance() {
		if (output == nullptr) {
			create();
		}
		return *output;
	}

	static void read_output(std::istream& in = std::cin) {
		output->create();
		output->read(in);
	}

	Output(const Output&) = delete;
	Output(Output&&) = delete;
};

std::unique_ptr<Input> Input::input = nullptr;
std::unique_ptr<Output> Output::output = nullptr;

// 戻り値: 納期遅れペナルティ、チェック: 品目と焼き時間の整合性 --- return: delay penalty, check: correctness of the items and the times to bake
cost_type bom_check() {
	const auto& input = Input::instance();
	const auto& output = Output::instance();
	auto sum = static_cast<cost_type>(0ULL);
	std::vector<std::size_t> bom_table(input.header.I + 1ULL);
	auto ipre = -1;
	std::size_t index = 0U;
	for (const auto& b : input.bom.vec) {
		if (ipre != static_cast<int64_t>(b.i)) {
			ipre = static_cast<int64_t>(b.i);
			bom_table[b.i] = index;
		}
		index++;
	}
	bom_table.back() = input.header.BL;
	for (const auto& o : output.order.vec) {
		auto m = o.m;
		auto t1 = o.t1;
		auto t2 = o.t2;
		auto i = input.order.vec[o.r].i;
		auto d = input.order.vec[o.r].d;
		auto pr = input.order.vec[o.r].pr;
		auto a = input.order.vec[o.r].a;
		auto q = input.order.vec[o.r].q;
		auto first = std::begin(input.bom.vec) + bom_table[i];
		auto last = std::begin(input.bom.vec) + bom_table[i + 1ULL];
		auto it = std::find_if(first, last, [m](const Input::Bom::BomSub& b) { return b.m == m; });
		if (it == last) {		// 製造不能 --- impossible to bake
			std::string s("Output Data is wrong. Bread of i=");
			s += std::to_string(i) + " cannot be made by machine m=" + std::to_string(m) + "\nline : " + std::to_string(o.line_id);
			throw std::logic_error(s);
		}
		if (t2 <= t1 || it->t != t2 - t1) {		// 製造時間違い --- incorrect baking time
			std::string s("Output Data is wrong. t4 - t3 is not match with BOM.\nline : ");
			s += std::to_string(o.line_id);
			throw std::logic_error(s);
		}
		if (t1 < input.order.vec[o.r].e) {		// 最早開始時刻違反 --- violate the earliest start time
			std::string s("Output Data is wrong. Too early t3.\nline : ");
			s += std::to_string(o.line_id);
			throw std::logic_error(s);
		}

		
		if (t2 > d) {		// 納期遅れペナルティ --- delay penalty
			if (pr > std::numeric_limits<cost_type>::max() / o.q) {
				throw std::overflow_error("Overflow");
			}
			auto cost = pr * o.q;
			if (cost <= std::numeric_limits<cost_type>::max() / (t2 - d)) {
				cost *= (t2 - d);
			}
			else {
				throw std::overflow_error("Overflow");
			}
			if (a > std::numeric_limits<cost_type>::max() / q) {
				throw std::overflow_error("Overflow");
			}
			auto c1 = cost / (a * q);
			auto c2 = cost % (a * q) == 0 ? 0ULL : 1ULL;
			if (c1 <= std::numeric_limits<cost_type>::max() - c2 && sum <= std::numeric_limits<cost_type>::max() - c1 - c2) {
				sum += c1 + c2;
			}
			else {
				throw std::overflow_error("Overflow");
			}
		}
	}
	return sum;
}

// 戻り値: {段取りコスト, 電力コスト}、チェック: 前後の品目組み合わせと段取り時間の整合性及び休日と計画の整合性
// return: {setup cost, electric cost}, check: correctness of adjacent items combination and setup time, as well as correctness of holiday and operations
std::pair<cost_type, cost_type> combi_machine_check() {
	cost_type esum = 0ULL, ssum = 0ULL;
	const auto& input = Input::instance();
	const auto& output = Output::instance();
	auto oit = std::begin(output.order.vec);
	auto ipre = static_cast<int64_t>(-1);
	auto mpre = static_cast<machine_type>(0);
	auto cit = std::begin(output.combi.vec);
	auto last_time = static_cast<time_type>(0);
	while (oit != std::end(output.order.vec)) {
		const auto& o = *oit;
		auto r = o.r;
		auto m = o.m;
		if (mpre != m) {
			ipre = -1LL;
			last_time = 0ULL;
			mpre = m;
		}
		auto ot1 = o.t1;
		auto ot2 = o.t2;
		auto i = input.order.vec[r].i;
		// 段取り --- setup
		if (ipre != -1) {
			auto it = input.combi.vec[m].find(std::make_pair(static_cast<item_type>(ipre), i));
			if (it != std::end(input.combi.vec[m]) && it->second.first != static_cast<cost_type>(0)) {	// 段取り必要 --- setup time is required.
				auto ct1 = cit->t1, ct2 = cit->t2;
				if (cit->m != m || ct2 > ot1 || last_time > ct1) {		// 段取り時間が範囲外 --- the setup time is out of range.
					std::string s = "Output Data is wrong. No setup time before the order of line : ";
					s += std::to_string(o.line_id);
					throw std::logic_error(s);
				}
				if (ct2 <= ct1 || ct2 - ct1 != it->second.first) {		// 段取り時間の長さが違う --- incorrect length of time
					std::string s = "Output Data is wrong. Setup time is not match with COMBI.\nline : ";
					s += std::to_string(cit->line_id);
					throw std::logic_error(s);
				}
				auto cd1 = sec_to_day_down(ct1);
				auto cd2 = sec_to_day_up(ct2);
				auto itr1 = std::lower_bound(std::begin(input.machine.vec[m]), std::end(input.machine.vec[m]), cd1);
				auto itr2 = std::lower_bound(std::begin(input.machine.vec[m]), std::end(input.machine.vec[m]), cd2);
				if (itr1 < itr2) {		// 段取り時間が休日と被っている --- setup time overlaps with holiday.
					std::string s = "Output Data is wrong. Setup time is overlapping with break time.\nline : ";
					s += std::to_string(cit->line_id);
					throw std::logic_error(s);
				}
				if (ssum <= std::numeric_limits<cost_type>::max() - it->second.second) {		// 段取りコスト --- setup cost
					ssum += it->second.second;
				}
				else {
					throw std::overflow_error("Overflow");
				}
				last_time = ct1;
				++cit;
			}
		}

		// 焼き --- baking
		cost_type q = 0ULL;
		std::set<line_type> order_set;
		if (ot1 < last_time) {		// 焼き時間がかぶっている --- operating time overlaps with other operating time.
			throw std::logic_error("Output Data is wrong. Operating time overlaps with other operating time.");
		}
		while (oit != std::end(output.order.vec) && oit->t1 == ot1) {
			if (input.order.vec[oit->r].i != i) {
				std::string s = "Output Data is wrong. Operating time is wrong.\nline : ";
				s += std::to_string(oit->line_id);
				throw std::logic_error(s);
			}
			if (!order_set.insert(oit->r).second) {
				std::string s = "Output Data is wrong. Order number cannot appear twice at same operating time.\nline : " + std::to_string(oit->line_id);
				throw std::logic_error(s);
			}
			if (q <= std::numeric_limits<cost_type>::max() - oit->q) {
				q += oit->q;
			}
			else {
				throw std::overflow_error("Overflow");
			}
			++oit;
		}
		if (q > input.machine.rq[m]) {			// パン焼き機のキャパシティオーバー --- the capacity of the machine is over
			throw std::logic_error("Output Data is wrong. Capacity over in machine No." + std::to_string(m));
		}
		auto od1 = sec_to_day_down(ot1);
		auto od2 = sec_to_day_up(ot2);
		auto itr1 = std::lower_bound(std::begin(input.machine.vec[m]), std::end(input.machine.vec[m]), od1);
		auto itr2 = std::lower_bound(std::begin(input.machine.vec[m]), std::end(input.machine.vec[m]), od2);
		if (itr1 < itr2) {	// 休日中 --- during holiday
			std::string s = "Output Data is wrong. Operating time is overlapping with break time.\nline : ";
			s += std::to_string(oit->line_id);
			throw std::logic_error(s);
		}

		auto ec = std::lower_bound(std::begin(input.bom.vec), std::end(input.bom.vec), Input::Bom::BomSub{i, m, 0, 0},
			[](const Input::Bom::BomSub& a, const Input::Bom::BomSub& b) {return a.i < b.i || (a.i == b.i && a.m < b.m); })->ec;
		if (esum <= std::numeric_limits<cost_type>::max() - ec) {		// 電力コスト --- electric cost
			esum += ec;
		}
		else {
			throw std::overflow_error("Overflow");
		}
		ipre = static_cast<int64_t>(i);
		last_time = ot2;
	}
	if (cit != std::end(output.combi.vec)) {
		throw std::logic_error("Output Data is wrong. Too many COMBI.\nline : " + std::to_string(cit->line_id));
	}
	return {esum, ssum};
}

// 戻り値: 無し、チェック: 全オーダーこなしているか --- return: void, check: whether all of the order is assigned or not.
void order_check() {
	const auto& input = Input::instance();
	const auto& output = Output::instance();
	std::vector<cost_type> vec(input.header.R, 0ULL);
	for (const auto& o : output.order.vec) {
		if (vec[o.r] <= std::numeric_limits<cost_type>::max() - o.q) {
			vec[o.r] += o.q;
		}
		else {
			throw std::overflow_error("Overflow");
		}
	}
	if (std::any_of(std::begin(vec), std::end(vec), [&input, index = 0U](cost_type q) mutable {return q != input.order.vec[index++].q; })) {
		throw std::logic_error("Output Data is wrong. Quantity is not match with ORDER.");
	}
}

// 粗利合計 --- profit sum
cost_type culc_profit() {
	const auto& input = Input::instance();
	auto profit = std::accumulate(std::begin(input.order.vec), std::end(input.order.vec), 0ULL, [](cost_type c, const Input::Order::OrderSub& o) noexcept(false) { 
		if (c > std::numeric_limits<cost_type>::max() - o.pr) {
			throw std::overflow_error("Overflow");
		}
		return c + o.pr;
	});
	return profit;
}

cost_type check() {
	auto delay_penalty = bom_check();
	auto penalty = delay_penalty;
	auto cost = combi_machine_check();
	if (penalty <= std::numeric_limits<cost_type>::max() - cost.first && penalty + cost.first <= std::numeric_limits<cost_type>::max() - cost.second) {
		penalty += cost.first + cost.second;
	}
	else {
		throw std::overflow_error("Overflow");
	}
	order_check();
	auto profit = culc_profit();
	if (profit > std::numeric_limits<cost_type>::max() - base_profit) {
		throw std::overflow_error("Overflow");
	}
#ifdef LOCAL_TEST
	std::cout << profit << "," << delay_penalty << "," << cost.first << "," << cost.second;
#endif // LOCAL_TEST
	profit += base_profit;
	return profit - 1ULL >= penalty ? profit - penalty : 1ULL;
}

cost_type culc_point(cost_type p) {
	auto logp = std::log10(p);
	if (logp > std::numeric_limits<double>::max() / point_rate - 1.) {
		throw std::overflow_error("Overflow");
	}
	auto ceiled = std::ceil(logp * point_rate);
	if (ceiled > static_cast<double>(std::numeric_limits<cost_type>::max())) {
		throw std::overflow_error("Overflow");
	}
	return static_cast<cost_type>(ceiled);
}

int main(int argc, const char* argv[]) {
	try {
		if (argc != 4) {
			throw std::logic_error(std::string("Arguments error.\nusage:\n>") + std::string(argv[0]) + std::string(" [input file path] [user output file path] [expected output file path]"));
		}
		std::ifstream in(argv[1], std::ios::in), out(argv[2], std::ios::in);
		if (!in || !out) {
			throw std::logic_error("The file cannot open.");
		}
		Input::read_input(in);
		Output::read_output(out);
		auto profit = check();
		auto point = culc_point(profit);
#ifdef LOCAL_TEST
		std::cout << "," << point << std::endl;
#else // ^^^ defined(LOCAL_TEST) ^^^ // vvv !defined(LOCAL_TEST) vvv
		std::cout << "IMOJUDGE<<<" << point << ">>>" << std::endl;
#endif // LOCAL_TEST
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	catch (...) {
		std::cerr << "error" << std::endl;
		return 1;
	}
	return 0;
}

