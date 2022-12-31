import click
import parse
import copy
from typing import Dict, List, Set, Tuple
from typing_extensions import TypeAlias, TypedDict


class ValveInfo(TypedDict):
    flowrate: int
    connections: List[str]
    is_open: bool


ValveDict: TypeAlias = Dict[str, ValveInfo]
ConnectionDict: TypeAlias = Dict[str, Dict[str, int]]
PathHistory: TypeAlias = List[Tuple[str, int]]



# TODO: Move to bottom
@click.group()
def cli() -> None:
    """Main entry to CLI."""


def parse_file(input_filepath: str) -> ValveDict:
    valve_dict = {}
    with open(input_filepath, mode="r", encoding="utf-8") as inputfile:
        for line in inputfile:
            line = line.strip()
            if not line:
                continue
            flowrate_str, tunnels_str = line.split("; ")
            parse_result = parse.parse(
                "Valve {name:S} has flow rate={flowrate:d}", flowrate_str
            )
            tunnels = tunnels_str[22:].strip().split(", ")
            valve_dict[parse_result["name"]] = {
                "flowrate": parse_result["flowrate"],
                "connections": tunnels,
                "is_open": False,
            }
    return valve_dict


def find_shortest_route(
    current_valve: str,
    end_valve: str,
    valve_dict: ValveDict,
    valves_visited: Set[str],
    current_best: int,
) -> int:
    tracked_best = current_best
    if current_valve == end_valve:
        return len(valves_visited)
    if len(valves_visited) > tracked_best:
        return tracked_best
    if current_valve in valves_visited:
        return tracked_best

    new_valves_visited = valves_visited | {current_valve}
    for valve in valve_dict[current_valve]["connections"]:
        temp_best = find_shortest_route(
            valve, end_valve, valve_dict, new_valves_visited, tracked_best
        )
        tracked_best = temp_best if temp_best < tracked_best else tracked_best
    return tracked_best


def calculate_routes(valve_dict: ValveDict, first_valve: str) -> ConnectionDict:
    connection_dict = {}
    nonzero_keys = set(
        [valve for valve in valve_dict.keys() if valve_dict[valve]["flowrate"] > 0]
    )
    applicable_keys = nonzero_keys | {first_valve}
    for valve_start in applicable_keys:
        target_valves = applicable_keys - {valve_start}
        pathway_dict = {}
        for valve_end in target_valves:
            if (
                valve_end in connection_dict
                and valve_start in connection_dict[valve_end]
            ):
                pathway_dict[valve_end] = connection_dict[valve_end][valve_start]
            else:
                shortest_route = find_shortest_route(
                    valve_start, valve_end, valve_dict, set(), len(target_valves) + 1
                )
                pathway_dict[valve_end] = shortest_route
        connection_dict[valve_start] = pathway_dict
    return connection_dict


def calculate_max_release(valve_dict: ValveDict, connection_dict: ConnectionDict, current_valve: str, path_history: PathHistory, time_left: int, current_released: int, current_rate: int) -> Tuple[int, PathHistory]:
    max_released = current_released
    unopened_valves = [valve for valve in connection_dict[current_valve] if not valve_dict[valve]["is_open"]]
    max_rate = current_rate
    if not unopened_valves:
        new_released = current_released + (current_rate * time_left)
        return new_released, path_history
    max_path_history = copy.deepcopy(path_history)
    max_path_history += [(current_valve, 31-time_left)]
    temp_path_history = copy.copy(max_path_history)
    for valve in unopened_valves:
        # Handle if time is less than move
        time_to_arrive = connection_dict[current_valve][valve]
        if time_left <= time_to_arrive + 2:
            new_released = current_released + (current_rate * time_left)
            max_released = new_released if new_released > max_released else max_released
        # Handle move if enough time
        else:
            new_released = current_released + (current_rate * (time_to_arrive + 1))
            new_time_left = time_left - (time_to_arrive + 1)
            new_rate = current_rate + valve_dict[valve]["flowrate"]
            new_valve_dict = copy.deepcopy(valve_dict)
            new_valve_dict[valve]["is_open"] = True
            nested_new_released, nested_path_history = calculate_max_release(new_valve_dict, connection_dict, valve, temp_path_history, new_time_left, new_released, new_rate)
            if nested_new_released > max_released:
                max_released = nested_new_released
                max_path_history = nested_path_history
                #print(f"QQQ - {nested_path_history}")
            max_rate = new_rate if new_rate > max_rate else max_rate
    #print(new_path_history)
    return max_released, max_path_history


@cli.command()
@click.argument("input_filepath")
def part1(input_filepath: str) -> None:
    valve_dict = parse_file(input_filepath)
    connection_dict = calculate_routes(valve_dict, "AA")
    import pprint
    pprint.pprint(connection_dict)
    print(calculate_max_release(valve_dict, connection_dict, "AA", [], 30, 0, 0))


if __name__ == "__main__":
    cli()
