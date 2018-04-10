import events
import re


def invert_main():
    mer_file_path = "invert_mer_file/"
    mevents = events.Events(mer_file_path)
    for event in mevents.events:
        with open(mer_file_path + event.file_name, 'r') as f:
            content = f.read()
        environment = re.findall("<ENVIRONMENT>.+</PARAMETERS>", content, re.DOTALL)[0]
        event.invert_transform(environment)
        event.correct_date()
        event.plotly(mer_file_path)
        event.plot(mer_file_path)


if __name__ == "__main__":
    invert_main()
