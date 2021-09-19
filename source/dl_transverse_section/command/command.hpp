class Command {
public:
	virtual ~Command() = default;
	virtual void execute() const = 0;
};