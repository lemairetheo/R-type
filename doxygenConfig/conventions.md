# Coding Conventions

## 1. Commit Message Guidelines

To maintain a clean, understandable, and consistent Git history, follow the conventions below for your commit messages.

### Format

Each commit message must adhere to the following format:

```plaintext
<type>(<scope>): <description>
```

- **type**: Specifies the type of change. Use one of the following:
    - `feat`: New feature implementation.
    - `fix`: Bug fix or error correction.
    - `remove`: Code or feature removal.
    - `clean`: Code refactoring or cleanup.
- **scope**: Indicates which part of the project the commit affects:
    - `client`: Changes in the client-side code.
    - `server`: Changes in the server-side code.
- **description**: A short, imperative sentence describing the change.

### Examples

- `feat(client): player can shoot`
- `fix(server): resolve crash on client disconnect`
- `remove(client): delete unused assets`
- `clean(server): refactor networking class for readability`

---

## 2. Creating Issues in GitHub Project

All development tasks must be tracked using issues. The issue title and body should follow this template:

### Issue Title Format
```
<SCOPE>-<Short Description>
```

**Examples:**
- `CLIENT-Create class input`
- `SERVER-Handle player disconnection`

### Issue Body Template

```
### Description
Provide a clear and concise explanation of the task or problem to solve.

### Tasks
- [ ] List all subtasks required to complete the issue.

### Acceptance Criteria
- Define the specific conditions that must be met for this issue to be marked as complete.
```

### Example Issue

| Assignees        | Status   | Size | Labels       | Start Date  | End Date    |
|------------------|----------|------|--------------|-------------|-------------|
| `romaincollignon` | `To Do`  | `M`  | `enhancement` | `Dec 3, 2024` | `Dec 5, 2024` |

**Issue Title:**  
`CLIENT-Create class input`

**Issue Body:**
```
### Description
Implement the `Input` class responsible for handling user inputs (keyboard and mouse) in the client-side logic.

### Tasks
- [ ] Create the Input class.
- [ ] Integrate it into the game loop.
- [ ] Add support for configurable key bindings.

### Acceptance Criteria
- Input class captures keyboard and mouse events.
- Key bindings are configurable.
- Smooth integration into the client game loop.
```

---

## 3. Workflow Guidelines

### 3.1 Branch Naming Convention

Each branch **must** be connected to an issue and follow the naming convention below:

```plaintext
<type>/<scope>/<issue-title>
```

**Examples:**
- Issue: `CLIENT-Create class input` → Branch: `feat/client/create-class-input`
- Issue: `SERVER-Handle player disconnection` → Branch: `fix/server/handle-player-disconnection`

---

### 3.2 Pull Requests

Pull requests (PRs) are mandatory for merging branches into the main codebase. Follow these guidelines for naming and content:

#### PR Title Format
The PR title must follow the commit convention:

```plaintext
<type>(<scope>): <description>
```

**Example:**
- `feat(client): player can shoot`

#### PR Body Template
Include a clear description of the changes made:

```
### Description
Briefly describe the purpose of the pull request and the changes introduced.

### Related Issue
Closes #<issue-number>

### Tasks Completed
- [ ] List all completed tasks here.

### Testing
Describe the testing performed, if applicable.
```

---

### 3.3 Workflow Summary

1. **Create an Issue**: Define the task with a clear title and description.
2. **Create a Branch**: Use the issue title to name the branch (`feat/client/create-class-input`).
3. **Commit Changes**: Use the standard commit format (`feat(client): add input handling`).
4. **Open a Pull Request**:
    - Link the PR to the issue (`Closes #<issue-number>`).
    - Ensure it is reviewed before merging.
5. **Merge**: After approval, merge the branch into the main codebase.

---

By following these conventions, we ensure a clean, traceable, and organized workflow for the project. 🚀