new Vue({
    el: '#app',
    data: {
        users: [],  // 用于存储所有用户信息
        transactions: []  // 用于存储所有交易流水信息
    },
    methods: {
        // 退出功能
        goToLogout() {
            axios.post('/logout')  // 调用后端的登出接口
                .then(response => {
                    alert('登出成功');
                    window.location.href = '/login';  // 跳转到登录页面
                })
                .catch(error => {
                    alert('登出失败，请稍后再试');
                });
        },

        // 查询所有用户信息
        getAllUsers() {
            window.location.href = '/admin/users';
        },

        // 查询所有交易流水信息
        getAllTransactions() {
            window.location.href = '/admin/transactions';
        }
    }
});