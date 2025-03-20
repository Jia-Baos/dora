#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from dora import Node, DoraStatus

class Operator:
    """Get message from C++ node
    """

    def __init__(self):
        self.bboxs = []
        self.buffer = ""
        self.submitted = []
        self.lines = []

    def on_event(
        self,
        dora_event,
        send_output,
    ):
        if dora_event["type"] == "INPUT":
            id = dora_event["id"]
            value = dora_event["value"]
            value_len = len(value)
            print("[Python node] received continue event")
            print(f"""Node received: id: {id}, value: {value}, value_len: {value_len}""")

        elif dora_event["type"] == "STOP":
            print("[Python node] received stop event")
            return DoraStatus.STOP

        return DoraStatus.CONTINUE

if __name__ == "__main__":
    node = Node()
    operator = Operator()
    for event in node:
        status = operator.on_event(event, node.send_output)
        if status == DoraStatus.STOP:
            break
